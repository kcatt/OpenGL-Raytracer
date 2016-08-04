#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Utilities.h"
#include "SDL.h"
#include <GL/glut.h>

class Camera {
 private:
    Point3 eye, look;
    Vector3 u, v, n;
    double viewAngle, aspect, nearDist, farDist;
    void setModelviewMatrix();

 public:
    Camera();
    void set (Point3 Eye, Point3 Look, Vector3 up);
    void roll (GLdouble angle);
    void pitch (GLdouble angle);
    void yaw (GLdouble angle);
    void slide (GLdouble delU, GLdouble delV, GLdouble delN);
    void setShape (GLdouble vAng, GLdouble asp, GLdouble nearD, GLdouble farD);
    void getShape (GLdouble &vAng, GLdouble &asp,
                   GLdouble &nearD, GLdouble &farD);
    void calcDirection(Ray & theRay, double nCols, double nRows, double r, double c);
    void raytrace(Scene& scn, int blockSize, int nRows, int nCols);
};

#endif
