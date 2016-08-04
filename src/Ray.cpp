#include "Ray.h"

Ray::Ray() {
    start.set(0,0,0);
    dir.set(0,0,0);
    numInside = 0;
}

Ray::Ray(Point3 & origin) {
    start.set(origin);
    dir.set(0,0,0);
    numInside = 0;
}

Ray::Ray(Point3 & origin, Vector3 & direction) {
    start.set(origin);
    dir.set(direction);
    numInside = 0;
}

void Ray::setStart(const Point3 & p) {
    start.set(p);
}

void Ray::setDir(const Vector3 & v) {
    dir.set(v);
}

Point3 Ray::rayPos(double t) {
    Vector3 temp(this->dir*t);
    Point3 result(start, temp.x, temp.y, temp.z);
    return result;
}
