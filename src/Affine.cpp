#include "Affine.h"

/* Affine4  methods start here. */
Affine4::Affine4() {
    setIdentityMatrix();
}

void Affine4::setIdentityMatrix() {
    for (int i = 0; i < 16; i++) {
        if (i % 5 == 0)
            matrix[i] = 1;
        else
            matrix[i] = 0;
    }
}

void Affine4::set(Affine4 a) {
    for (int i = 0; i < 16; i++)
        matrix[i] = a.matrix[i];
}

void Affine4::preMult(Affine4 n) {
    GLdouble sum;
    Affine4 tmp;
    tmp.set(*this);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            sum = 0;
            for (int k = 0; k < 4; k++)
                sum += n.matrix[4 * k + j] * tmp.matrix[4 * i + k];
            matrix[4 * i + j] = sum;
        }
    }
}

void Affine4::postMult(Affine4 n) {
    GLdouble sum;
    Affine4 tmp;
    tmp.set(*this);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            sum = 0;
            for (int k = 0; k < 4; k++)
                sum += tmp.matrix[4 * k + j] * n.matrix[4 * i + k];
            matrix[4 * i + j] = sum;
        }
    }
}

void Affine4::applyTransform(float v[4]) {
    float q[4];
    for(int i=0;i<4; i++) {
        q[i] = 0;
        for(int k=0; k < 4; k++) {
            q[i] += matrix[4*k + i] * v[k];
        }
    }
    for(int ii=0; ii<4; ii++)
        v[ii] = q[ii];
    return;
}

void Affine4::transpose(Affine4& t) {
    for (int i = 0; i < 4; i++) {
        for (int k = 0; k < 4; k++) {
            t.matrix[4*i + k] = matrix[4*k + i];
        }
    }
}


/* AffineStack methods are here */
void AffineStack::duplicate() {
    AffineNode temp;
    tos.insert(tos.begin(), tos[0]);
}

void AffineStack::setIdentity() {
    assert (!tos.empty());
    tos[0].affn.setIdentityMatrix();
    tos[0].invAffn.setIdentityMatrix();
}

void AffineStack::popAndDrop() {
    if (tos.empty())
        return;
    tos.erase(tos.begin());
}

void AffineStack::releaseAffines() {
    while (!tos.empty())
        popAndDrop();
}

void AffineStack::rotate(float angle, Vector3 u) {
    Affine4 rm;
    Affine4 invRm;
    u.normalize(); // Make the rotation axis unit length
    float ang = angle * 3.14159/180; // Degrees to radians
    float c = cos(ang), s = sin(ang);
    float mc = 1.0 - c;

    rm.matrix[0] = c + mc * u.x * u.x;
    rm.matrix[1] = mc * u.x * u.y + s * u.z;
    rm.matrix[2] = mc * u.x * u.z - s * u.y;
    rm.matrix[4] = mc * u.y * u.x - s * u.z;
    rm.matrix[5] = c + mc * u.y * u.y;
    rm.matrix[6] = mc * u.y * u.z + s * u.x;
    rm.matrix[8] = mc * u.z * u.x + s * u.y;
    rm.matrix[9] = mc * u.z * u.y - s * u.x;
    rm.matrix[10] = c + mc * u.z * u.z;

    invRm.matrix[0] = c + mc * u.x * u.x;
    invRm.matrix[1] = mc * u.x * u.y - s * u.z;
    invRm.matrix[2] = mc * u.x * u.z + s * u.y;
    invRm.matrix[4] = mc * u.y * u.x + s * u.z;
    invRm.matrix[5] = c + mc * u.y * u.y;
    invRm.matrix[6] = mc * u.y * u.z - s * u.x;
    invRm.matrix[8] = mc * u.z * u.x - s * u.y;
    invRm.matrix[9] = mc * u.z * u.y + s * u.x;
    invRm.matrix[10] = c + mc * u.z * u.z;

    tos.front().affn.postMult(rm);
    tos.front().invAffn.preMult(invRm);
}

void AffineStack::scale(float sx, float sy, float sz) {
    const double sEps = 0.00001;
    Affine4 scl;
    Affine4 invScl;
    // Make a scaling matrix
    scl.matrix[0]  = sx;
    scl.matrix[5]  = sy;
    scl.matrix[10] = sz;
    if(fabs(sx) < sEps || fabs(sy) < sEps || fabs(sz) < sEps) {
        cerr << "Degenerate scaling transformation!\n";
    }
    invScl.matrix[0]  = 1/sx;
    invScl.matrix[5]  = 1/sy;
    invScl.matrix[10] = 1/sz;
    tos[0].affn.postMult(scl);
    tos[0].invAffn.preMult(invScl);
}

void AffineStack::translate(Vector3 d) {
    Affine4 tr;
    Affine4 invTr;
    tr.matrix[12] = d.x;
    tr.matrix[13] = d.y;
    tr.matrix[14] = d.z;
    invTr.matrix[12] = -d.x;
    invTr.matrix[13] = -d.y;
    invTr.matrix[14] = -d.z;
    tos[0].affn.postMult(tr);
    tos[0].invAffn.preMult(invTr);
}
