#ifndef __MESH_H_
#define __MESH_H_

#include "Shapes.h"

/* Classes used to define a Mesh */
// VertexID class
class VertexID {
public:
    int vertIndex;
    int normIndex;
};

// Face class
class Face {
public:
    int nVerts;
    VertexID * vert; // An array of vertex and normal indices

    Face() {
        nVerts = 0;
        vert = NULL;
    }

    ~Face() {
        delete[] vert;
        nVerts = 0;
    }
};

// Mesh class
class Mesh : public Shape {
public:
    enum RenderMode {
        MODE_WIRE = 0,
        MODE_SOLID,
        MODE_WIRE_SOLID
    };

    string meshFileName; // Holds the file name for this Mesh

    Mesh();
    Mesh(const string & file);

    void drawEdges();
    void drawFaces();
    virtual void drawOpenGL();
    void freeMesh();
    bool isEmpty();
    void makeEmpty();
    Vector3 newell4(int indx[]);
    void readMesh(const string & file);
    void setRenderMode(RenderMode m);
    void setScale(float s);

private:
    int numVerts, numNorms, numFaces;
    Point3 *points;
    Vector3 *normals;
    Face *faces;
    int lastVertUsed;
    int lastNormUsed;
    int lastFaceUsed;
    float scale;
    RenderMode mode;
}; // end of Mesh class

#endif
