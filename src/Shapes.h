#ifndef __SHAPES_H_
#define __SHAPES_H_

#include "Affine.h"
#include "Ray.h"
#include "pixmap.h"

// Material class
class Material {
public:
    Color3 ambient, diffuse, specular, emissive;
    int numParams;
    GLdouble params[10];
    int textureType;
    float specularExponent, reflectivity, transparency, speedOfLight;
    float specularFraction, surfaceRoughness;

    void set(const Material & m);

    void setDefault();
};

// Geometric Object class
class GeomObj {
public:
    // Declare polymorphic methods
    virtual void drawOpenGL(){ }

    virtual void loadStuff(){}

    virtual void tellMaterialsGL(){}

    virtual bool hit(Ray& r, Intersection& inter) {return false;}
/*
private:
    SphereInfo genSphereExtent, worldSphereExtent;
    Cuboid genBoxExtent, worldBoxExtent;*/
};

// Shape class
class Shape: public GeomObj {
public:
    Material mat;
    Affine4 transf, invTransf;

    bool textureExists;

    Pixmap pixmap[6];

    Shape() {
        mat.textureType = 0;
        mat.numParams = 0;
        textureExists = false;
    }

    virtual void drawOpenGL() {}

    virtual bool hit(Ray& r, Intersection& inter) {return false;}

    void setMaterial (const Material & mtrl) {
        mat.set(mtrl);
    }

    void xfrmRay(Ray& genRay, Ray& r);

    void tellMaterialsGL();
};

// Square class
class Square : public Shape {
public:
    Square() {}

    void drawOpenGL();
    bool hit(Ray& r, Intersection& inter);
};

// Cube class
class Cube : public Shape {
public:
    Cube(){}

    void drawOpenGL();
    bool hit(Ray& r, Intersection& inter);
    Vector3 cubeNormal(int which);
};

// Sphere class
class Sphere : public Shape {
public:
    Sphere() { }

    void drawOpenGL();
    bool hit(Ray& r, Intersection& inter);
};

// TaperedCylinder class
// A radius of 1 is a cylinder and a radius of 0 is a cone
class TaperedCylinder : public Shape {
private:
    GLUquadricObj * pwall; // OpenGL needs pointers to quadric objects
    GLUquadricObj * pbase;
    GLUquadricObj * pcap;

public:
    float smallRadius;

    TaperedCylinder() {
        // Make the quadrics once only.
        pwall = gluNewQuadric(); // Make a cylinder
        pbase = gluNewQuadric(); // Make a cylinder
        pcap  = gluNewQuadric(); // Make a cylinder
        gluQuadricDrawStyle(pwall,GLU_FILL);
        gluQuadricDrawStyle(pbase,GLU_FILL);
        gluQuadricDrawStyle(pcap, GLU_FILL);
        gluQuadricTexture(pwall, GL_TRUE);
    }

    void drawOpenGL();
    bool hit(Ray& r, Intersection& inter);
};

// Torus class
class Torus : public Shape {
public:
    void drawOpenGL() {
        tellMaterialsGL();
        glPushMatrix();
        glMultMatrixf(transf.matrix);
        glutSolidTorus(0.2,1.0,10,12);
        glPopMatrix();
    }
};

// Teapot class
class Teapot : public Shape {
public:
    void drawOpenGL() {
        tellMaterialsGL();
        glPushMatrix();
        glMultMatrixf(transf.matrix);
        glutSolidTeapot(1.0);
        glPopMatrix();
    }
};

#endif
