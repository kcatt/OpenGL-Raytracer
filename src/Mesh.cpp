#include "Mesh.h"

Mesh::Mesh() {
    numVerts = numFaces = numNorms = 0;
    points = NULL;
    normals  =  NULL;
    faces = NULL;
    lastVertUsed = lastNormUsed = lastFaceUsed = -1;
    scale = 1.0;
    mode = MODE_SOLID; // Default is to make solid mesh
}

Mesh::Mesh(const string & file) {
    numVerts = numFaces = numNorms = 0;
    points = NULL;
    normals  =  NULL;
    faces = NULL;
    lastVertUsed = lastNormUsed = lastFaceUsed = -1;
    scale = 1.0;
    mode = MODE_SOLID; // Default is to build with solid frame
    readMesh(file);
}

void Mesh::drawOpenGL() {
    tellMaterialsGL();
    glPushMatrix();
    glMultMatrixf(transf.matrix);

    switch (mode) {
    case MODE_WIRE:
        drawEdges();
        break;
    case MODE_SOLID:
        drawFaces();
        break;
    default: // MODE_WIRE_SOLID
        drawFaces();
        drawEdges();
        break;
    }

    glPopMatrix();
}

void Mesh::freeMesh() {
    delete [] points; // release whole vertex list
    delete [] normals;
    for(int i = 0; i < numFaces; i++)
        delete[] faces[i].vert; // delete the vert[] array of this face
    delete [] faces;
}

bool Mesh::isEmpty() {
    return (numVerts == 0) || (numFaces == 0) || (numNorms == 0);
}

void Mesh::makeEmpty() {
    numVerts = numFaces = numNorms = 0;
}

void Mesh::setRenderMode(RenderMode m) {
    mode = m;
}

void Mesh::setScale(float s) {
    scale = s;
}

Vector3 Mesh::newell4(int indx[]) {
    /* Return the normalized normal to face with vertices
     * pt[indx[0]],...,pt[indx[3]].
     * indx[] contains the four indices into the vertex list
     * to be used in the Newell calculation */
    Vector3 m;
    for(int i = 0; i < 4 ; i++) {
        int next = (i == 3) ? 0 : i + 1; // Get next index
        int f = indx[i], n = indx[next]; // Name the pair
        m.x += (points[f].y - points[n].y) * (points[f].z + points[n].z);
        m.y += (points[f].z - points[n].z) * (points[f].x + points[n].x);
        m.z += (points[f].x - points[n].x) * (points[f].y + points[n].y);
    }
    m.normalize();
    return m;
}

void Mesh::readMesh(const string & file) {
    ifstream input;
    input.open(file.c_str());

    if (input.fail ()) {
        cerr << "readMesh: File " << file<< " could not be opened." << endl;
        exit (1);
    }

    input >> numVerts >> numNorms >> numFaces;

    points = new Point3[numVerts];
    assert (points != NULL);
    normals = new Vector3[numNorms];
    assert (normals != NULL);
    faces = new Face[numFaces];
    assert (faces != NULL);

    for (int i = 0; i < numVerts; i++)
        input >> points[i].x >> points[i].y >> points[i].z;
    for (int i = 0; i < numNorms; i++)
        input >> normals[i].x >> normals[i].y >> normals[i].z;
    for (int i = 0; i < numFaces; i++) {
        input >> faces[i].nVerts;
        int n = faces[i].nVerts;
        faces[i].vert = new VertexID[n];
        for (int j = 0; j < n; j++)
            input >> faces[i].vert[j].vertIndex;
        for (int j = 0; j < n; j++)
            input >> faces[i].vert[j].normIndex;
    }

    input.close();
}

void Mesh::drawEdges() {
    if (this->isEmpty())
        return;
    for (int i = 0; i < numFaces; i++) {
        int n = faces[i].nVerts;
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < n; j++) {
            int vertNum = faces[i].vert[j].vertIndex;
            assert (vertNum >= 0 && vertNum < numVerts);
            glVertex3f(points[vertNum].x*scale,
                       points[vertNum].y*scale,
                       points[vertNum].z*scale);
        }
        glEnd();
    }
    glFlush();
}

void Mesh::drawFaces() {
    if (this->isEmpty())
        return;
    for (int i = 0; i < numFaces; i++) {
        int n = faces[i].nVerts;
        glBegin(GL_POLYGON);
        for (int j = 0; j < n; j++) {
            int normNum = faces[i].vert[j].normIndex;
            assert (normNum >= 0 && normNum < numNorms);
            glNormal3f(normals[normNum].x,
                       normals[normNum].y,
                       normals[normNum].z);
            int vertNum = faces[i].vert[j].vertIndex;
            assert (vertNum >= 0 && vertNum < numVerts);
            glVertex3f(points[vertNum].x*scale,
                       points[vertNum].y*scale,
                       points[vertNum].z*scale);
        }
        glEnd();
    }
    glFlush();
}
