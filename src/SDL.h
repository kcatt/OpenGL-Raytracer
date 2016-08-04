#ifndef _SDL_2
#define _SDL_2

#include <sstream>
#include "Utilities.h"
#include "pixmap.h"
#include "Ray.h"
#include "Affine.h"
#include "Shapes.h"
#include "Mesh.h"

// Enums for the tokens of the sdl file
enum mTokenType {
    IDENT, LIGHT, ROTATE, TRANSLATE,
    SCALE, PUSH, POP, IDENTITYAFFINE,
    GLOBALAMBIENT, BACKGROUND, MINREFLECTIVITY,
    MINTRANSPARENCY, MAXRECURSIONDEPTH, MAKEPIXMAP, CUBE,
    SPHERE, TORUS, PLANE, SQUARE, TAPEREDCYLINDER, TEAPOT,
    MESH, DEFAULTMATERIALS, AMBIENT, DIFFUSE,SPECULAR,
    SPECULARFRACTION, SURFACEROUGHNESS,EMISSIVE, SPECULAREXPONENT,
    SPEEDOFLIGHT, TRANSPARENCY,REFLECTIVITY, PARAMETERS, TEXTURE,
    LFTCURLY, RGHTCURLY, DEF, USE, T_NULL, F_EOF, UNKNOWN
};

// Light class
class Light {
public:
    Point3 pos;
    Color3 color;

    // Set the color of the light
    void setColor(Color3 c) {
        color.set(c);
    }

    // Set the position of the light
    void setPosition(Point3 p) {
        pos.set(p);
    }
};

// DefUnit class
class DefUnit {
public:
    string name, stuff;

    DefUnit (const string & n, const string & s) {
        stuff = s;
        name = n;
    }
};

// DefUnitStack class
class DefUnitStack {
public:
    DefUnitStack() { }

    void push (const string & n, const string & s);

    void print();

    bool search(const string & n);

    string contents(const string & s);

private:
    vector<DefUnit> defStack;
};

// Scene class
class Scene {
public:
    vector<Light> light;
    vector<GeomObj*> obj;
    Color3 background, ambient;
    int maxRecursionDepth;

    float minReflectivity, minTransparency;
    Pixmap pixmap[8];
    // Default construct - Sets light, obj, and tail to null
    Scene() {
        currMtrl.setDefault();
        background.set(.8,.3,0.6f);
        ambient.set(0.1f,0.1f,0.1f);
        minReflectivity = 0.5;
        minTransparency = 0.5;
        maxRecursionDepth = 3;
    }
    Scene(string fname) {
        Scene();
        read(fname);
    }

    ~Scene() {
        freeScene();
    }

    void drawSceneOpenGL();
    void setBackground(Color3& c);
    bool getObject();
    void freeScene();
    void makeLightsOpenGL();
    bool read(string fname);
    Color3 shade(Ray& ray);
    void getFirstHit(Ray& ray, Intersection& best);
    void lightingHelper(Material& mtrl, Point3& p, Vector3& v, Ray& r, Color3& clr);
    bool isInShadow(Ray& f);

private:
    int line;
    int nextline;
    ifstream  *file_in;
    stringstream *f_in;
    stringstream temp_fin;
    DefUnitStack *def_stack;
    AffineStack affStk; // Stack for transformations
    Material currMtrl;
    int numPixmaps = 0;
    bool textureExists = false;
    string nexttoken(void);
    float getFloat();
    bool isidentifier(string keyword);
    void cleanUp();
    mTokenType whichtoken(string keyword);

};

#endif
