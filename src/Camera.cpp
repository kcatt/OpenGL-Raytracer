#define _USE_MATH_DEFINES

#include "Camera.h"
#include <GL/glut.h>
#include <cmath>

Camera::Camera() {
    setShape(45.0, 640/480.0, .1, 200.0);
    set (Point3(5,5,5), Point3(0,0,0), Vector3(0,1,0));
}

void Camera::set (Point3 Eye, Point3 Look, Vector3 up) {
    eye.set(Eye);
    look.set(Look);

    n.set(eye.x-look.x, eye.y - look.y, eye.z - look.z);
    u.set(up.cross(n));
    n.normalize();
    u.normalize();
    v.set(n.cross(u));
    setModelviewMatrix();
}

void Camera::setModelviewMatrix() {
    GLfloat matrix[16];
    Vector3 eyeVector(eye.x, eye.y, eye.z);

    matrix[0] = u.x;
    matrix[4] = u.y;
    matrix[8] = u.z;
    matrix[12] = -eyeVector.dot(u);
    matrix[1] = v.x;
    matrix[5] = v.y;
    matrix[9] = v.z;
    matrix[13] = -eyeVector.dot(v);
    matrix[2] = n.x;
    matrix[6] = n.y;
    matrix[10] = n.z;
    matrix[14] = -eyeVector.dot(n);
    matrix[3] = 0;
    matrix[7] = 0;
    matrix[11] = 0;
    matrix[15] = 1;

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(matrix);
}

void Camera::roll (GLdouble angle) {
    GLfloat c = cos (3.14159/180 * angle);
    GLfloat s = sin (3.14159/180 * angle);

    Vector3 t (u);
    u.set (c*t.x - s*v.x, c*t.y - s*v.y, c*t.z - s*v.z);
    v.set (s*t.x + c*v.x, s*t.y + c*v.y, s*t.z + c*v.z);
    setModelviewMatrix();
}

void Camera::pitch (GLdouble angle) {
    GLfloat c = cos (3.14159/180 * angle);
    GLfloat s = sin (3.14159/180 * angle);

    Vector3 t (v);
    v.set (c*t.x - s*n.x, c*t.y - s*n.y, c*t.z - s*n.z);
    n.set (s*t.x + c*n.x, s*t.y + c*n.y, s*t.z + c*n.z);
    setModelviewMatrix();
}

void Camera::yaw (GLdouble angle) {
    GLfloat c = cos (3.14159/180 * angle);
    GLfloat s = sin (3.14159/180 * angle);

    Vector3 t (n);
    n.set (c*t.x - s*u.x, c*t.y - s*u.y, c*t.z - s*u.z);
    u.set (s*t.x + c*u.x, s*t.y + c*u.y, s*t.z + c*u.z);
    setModelviewMatrix();
}

void Camera::slide (GLdouble delU, GLdouble delV, GLdouble delN) {
    double delX = delU*u.x + delV*v.x + delN*n.x;
    double delY = delU*u.y + delV*v.y + delN*n.y;
    double delZ = delU*u.z + delV*v.z + delN*n.z;
    eye.set(eye.x+delX, eye.y+delY, eye.z+delZ);
    setModelviewMatrix();
}

void Camera::setShape (GLdouble vAng, GLdouble asp,
    GLdouble nearD, GLdouble farD) {
    viewAngle = vAng;
    aspect = asp;
    nearDist = nearD;
    farDist = farD;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(viewAngle, aspect, nearDist, farDist);
    glMatrixMode(GL_MODELVIEW);
}

void Camera::getShape (GLdouble &vAng, GLdouble &asp,
    GLdouble &nearD, GLdouble &farD) {
    vAng = viewAngle;
    asp = aspect;
    nearD = nearDist;
    farD = farDist;
}

void Camera::calcDirection(Ray & theRay, double nCols, double nRows, double r, double c) {
  double H, W;
  H = nearDist * tan(viewAngle * M_PI/360);
  W = H * aspect;
  theRay.dir = n*(-nearDist)+u*(W*((2.0*c)/nCols - 1.0))+v*(H*((2.0*r)/nRows - 1.0));
}

void Camera::raytrace(Scene & scn, int blockSize, int nRows, int nCols) {
  Ray theRay;
  theRay.setStart(eye);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, nCols, 0, nRows);
  glDisable(GL_LIGHTING);
  for (int row = 0; row < nRows; row+=blockSize) {
    for (int col = 0; col < nCols; col+=blockSize) {
      calcDirection(theRay, nCols, nRows, row, col);
      Color3 clr = scn.shade(theRay);
      glColor3f(clr.red, clr.green, clr.blue);
      glRecti(col, row, col+blockSize, row+blockSize);
      glFlush();
      glutSwapBuffers();
    }
  }
}
