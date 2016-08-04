#include "Utilities.h"

Point3 operator-(const Point3& pt, const Vector3& v) {
    Point3 p(pt);
    p.x -= v.x;
    p.y -= v.y;
    p.z -= v.z;
    return p;
}

ostream& operator<<(ostream& out, const Vector3& v) {
  out << "<" << v.x << ", " << v.y << ", " << v.z << ">";
  return out;
}

ostream& operator<<(ostream& out, const Point3& v) {
  out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
  return out;
}

ostream& operator<<(ostream& out, const Color3& c) {
    out << "(" << c.red << ", " << c.green << ", " << c.blue << ")";
    return out;
}

// Vector3 methods
// This crossed with b
Vector3 Vector3 :: cross(Vector3 b)
{
    Vector3 c(this->y*b.z - this->z*b.y,
              this->z*b.x - this->x*b.z,
              this->x*b.y - this->y*b.x);
    return c;
}

// This dotted with b
float Vector3 :: dot(Vector3 b) {
    return x * b.x + y * b.y + z * b.z;
}

// Make the vector normal
void Vector3 :: normalize() {
    double sizeSq = x*x+y*y+z*z;
    if(sizeSq < 0.0000001) {
        cerr << "\nnormalize() sees vector (0,0,0)!" << endl;
        return; // does nothing to zero vectors;
    }
    float scaleFactor = 1.0/(float)sqrt(sizeSq);
    this->x *= scaleFactor;
    this->y *= scaleFactor;
    this->z *= scaleFactor;
}

// Color3 methods
void Color3 ::add(const Color3 & src, const Color3 & refl) {
    // add the product of source color and reflection coefficient
    this->red   += (src.red * refl.red);
    this->green += (src.green * refl.green);
    this->blue  += (src.blue * refl.blue);
}

// Add colr to this color
void Color3:: add(const Color3 & colr) {
    this->red += colr.red;
    this->green += colr.green;
    this->blue += colr.blue;
}

// Make a 4 tuple with this color. v[3] is for homogeneous
void Color3 :: build4tuple(float v[]) {
    v[0] = this->red;
    v[1] = this->green;
    v[2] = this->blue;
    v[3] = 1.0f;
}
