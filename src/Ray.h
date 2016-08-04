#ifndef __RAY_H_
#define __RAY_H_

#include "Utilities.h"

class GeomObj;

class Ray {
public:
    Point3 start;
    Vector3 dir;
    int recurseLevel;
    int row, col;
    int numInside;
    GeomObj * inside[10];

    Ray();
    Ray(Point3 & origin);
    Ray(Point3 & origin, Vector3 & direction);

    void setStart(const Point3 & p);
    void setDir(const Vector3 & v);
    Point3 rayPos(double t);
};

class HitInfo {
public:
    double hitTime;
    GeomObj* hitObject;
    int surface;
    bool isEntering;
    Point3 hitPoint;
    Vector3 hitNormal;

    HitInfo() {
        hitObject = NULL;
        hitTime = -1000;
        surface = 0;
        isEntering = false;
    }
    void set(HitInfo& h) {
        hitTime = h.hitTime;
        hitObject = h.hitObject;
        surface = h.surface;
        isEntering = h.isEntering;
        hitPoint.set(h.hitPoint);
        hitNormal.set(h.hitNormal);
    }
};

class Intersection {
public:
#define maxNumHits 8
    int numHits;
    HitInfo hit[maxNumHits];

    Intersection() {
        numHits = 0;
    }
    void set(Intersection& inter) {
        numHits = inter.numHits;
        for (int i = 0; i < maxNumHits; i++) {
            hit[i].set(inter.hit[i]);
        }
    }
};

class SphereInfo {
public:
    Point3 center;
    float radSq;
    void set(float x, float y, float z, float rsq) {
        center.set(x,y,z);
        radSq = rsq;
    }
};

class Cuboid {
public:
    float left, top, right, bott, front, back;
    void set(float l, float t, float r, float b, float f, float bk) {
        left = l;
        top = t;
        right = r;
        bott = b;
        front = f;
        back = bk;
    }
    void set (const Cuboid& c) {
        left = c.left;
        top = c.top;
        right = c.right;
        bott = c.bott;
        front = c.front;
        back = c.back;
    }
};

class PointCluster {
public:
    int num;
    Point3* pts;

    PointCluster() {
        num = 0;
        pts = NULL;
    }

    PointCluster(int n) {
        num = n;
        pts = new Point3[n];
    }
};

#endif
