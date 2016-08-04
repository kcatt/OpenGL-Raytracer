#ifndef __AFFINE_H_
#define __AFFINE_H_

#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <cassert>
#include <GL/glut.h>
#include "Utilities.h"

using namespace std;

// Affine tranformation class
class Affine4 {
public:
    float matrix[16]; // hold a 4 by 4 matrix

    Affine4();

    void preMult(Affine4 n);

    void postMult(Affine4 n);

    void set(Affine4 a);

    void setIdentityMatrix();

    Vector3 applyTransform(Vector3& v);

    Point3 applyTransform(Point3& p);

    void applyTransform(float v[]);

    void transpose(Affine4& t);

};

// AffineNode class
class AffineNode {
public:
    Affine4 affn;
    Affine4 invAffn;

    AffineNode() { }

};

// AffineStack class
class AffineStack {
public:
    vector<AffineNode> tos;

    AffineStack() {
        AffineNode temp;
        tos.push_back(temp); // Initialize the vector with identity in it
    }

    void duplicate();

    void popAndDrop();

    void releaseAffines();

    void rotate (float angle, Vector3 u);

    void scale (float sx, float sy, float sz);

    void setIdentity();

    void translate (Vector3 d);
};

#endif
