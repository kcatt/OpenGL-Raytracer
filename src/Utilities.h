#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <cmath>
#include <iostream>
using namespace std;

//############################### class Point2 #######################
class Point2 {
 public:
    float x,y;

    // Constructor for two points
    Point2(float xx, float yy) {
        x = xx;
        y = yy;
    }

    // Default constructor
    Point2() {
        x = y = 0;
    }

    void set(float xx, float yy) {
        x = xx;
        y = yy;
    }
};

class Vector3;

//@@@@@@@@@@@@@@@@@@ Point3 class @@@@@@@@@@@@@@@@
class Point3 {
 public:
    float x,y,z;

    Point3(float xx, float yy, float zz) {
        x = xx;
        y = yy;
        z = zz;
    }

    Point3(Point3& p, float a, float b, float c) {
      x = p.x+a;
      y = p.y+b;
      z = p.z+c;
    }

    Point3() {
        x = y = z = 0;
    }

    void build4tuple(float v[]) {
        // load 4-tuple with this color: v[3] = 1 for homogeneous
        v[0] = x; v[1] = y; v[2] = z; v[3] = 1.0f;
    }

    void set(float dx, float dy, float dz) {
        x = dx;
        y = dy;
        z = dz;
    }

    void set(const Point3& p) {
        x = p.x;
        y = p.y;
        z = p.z;
    }

    void add(Point3& p) {
      x += p.x;
      y += p.y;
      z += p.z;
    }

    inline Point3 operator=(const Point3& v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
    
    friend Point3 operator-(const Point3& pt, const Vector3& v);

    friend ostream& operator<<(ostream& out, const Point3& v);
};

//@@@@@@@@@@@@@@@@@@ Vector3 class @@@@@@@@@@@@@@@@
class Vector3 {
 public:
    float x,y,z;

    Vector3(float xx, float yy, float zz) {
        x = xx;
        y = yy;
        z = zz;
    }

    // Make a vector from a vector
    Vector3(const Vector3 & v) {
        x = v.x;
        y = v.y;
        z = v.z;
    }

    // Default constructor
    Vector3() {
        x = y = z = 0;
    }

    // Cross product (this x b)
    Vector3 cross(Vector3 b);

    // This dotted with b
    float dot(Vector3 b);

    void build4tuple(float v[]) {
      // load 4-tuple with this color: v[3] = 1 for homogeneous
      v[0] = x; v[1] = y; v[2] = z; v[3] = 0.0f;
    }

    // Reverse the vector
    void flip() {
        x = -x;
        y = -y;
        z = -z;
    }

    // Make the vector normal (unit length)
    void normalize();

    // Set the value of the vector with 3 points
    void set(float dx, float dy, float dz) {
        x = dx;
        y = dy;
        z = dz;
    }

    // Set the value of the vector with 3 points
    void set(Point3& p) {
      x = p.x;
      y = p.y;
      z = p.z;
    }

    // Set the value of the vector with another vector
    void set(const Vector3 & v) {
        x = v.x;
        y = v.y;
        z = v.z;
    }

    // Make a vector from 2 points
    void setDiff(const Point3 & a, const Point3 & b) {
        x = a.x - b.x;
        y = a.y - b.y;
        z = a.z - b.z;
    }

    inline Vector3 operator*(const double& value) const {
      Vector3 temp(*this);
      temp.x *= value;
      temp.y *= value;
      temp.z *= value;
      return temp;
    }

    inline Vector3 operator+(const Vector3& v) const {
      Vector3 temp;
      temp.x = x + v.x;
      temp.y = y + v.y;
      temp.z = z + v.z;
      return temp;
    }

    inline Vector3 operator=(const Vector3& v) {
      x = v.x;
      y = v.y;
      z = v.z;
      return *this;
    }

    friend ostream& operator<<(ostream& out, const Vector3& v);

};

// @@@@@@@@@@@@@@@@@@@@@ Color3 class @@@@@@@@@@@@@@@@
class Color3 { // holds an red,green,blue 3-tuple
 public:
    float red, green, blue;

    // Default constructor
    Color3() {
        red = green = blue = 0;
    }

    // Set the color using three floats
    Color3(float r, float g, float b) {
        red = r;
        green = g;
        blue = b;
    }

    // Set the color using another color
    Color3(const Color3 & c) {
        red = c.red;
        green = c.green;
        blue = c.blue;
    }

    // Add to the color
    void add(float r, float g, float b) {
        red += r;
        green += g;
        blue += b;
    }

    void add(const Color3 & src, const Color3 & refl);

    void add(const Color3 & colr);

    void build4tuple(float v[]);

    // Set the color using 3 floats
    void set(float r, float g, float b) {
        red = r;
        green = g;
        blue = b;
    }

    // Set the color using another color
    void set(const Color3 & c) {
        red = c.red;
        green = c.green;
        blue = c.blue;
    }

    inline Color3 operator*(const double& value) const {
        Color3 temp(*this);
        temp.red *= value;
        temp.green *= value;
        temp.blue *= value;
        return temp;
    }

    inline Color3 operator*(const Color3& c) const {
        Color3 temp(*this);
        temp.red *= c.red;
        temp.green *= c.green;
        temp.blue *= c.blue;
        return temp;
    }

    inline Color3 operator=(const Color3& v) {
        red = v.red;
        green = v.green;
        blue = v.blue;
        return *this;
    }

    friend ostream& operator<<(ostream& out, const Color3& c);
};

#endif
