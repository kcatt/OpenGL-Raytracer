#include "Shapes.h"

double dot3(Point3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

double dot3(Point3 a, Point3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/* Material methods are here */
void Material::setDefault() {
    textureType = 0;
    numParams = 0;
    reflectivity = transparency = 0.0;
    speedOfLight = specularExponent = .5;
    specularFraction = 0.0;
    surfaceRoughness = 1.0;
    ambient.set(0.1 ,0.1, 0.1);
    diffuse.set(0.8, 0.8, 0.8);
    specular.set(0, 0, 0);
    emissive.set(0, 0, 0);
}

void Material::set(const Material & m) {
    textureType = m.textureType;
    numParams = m.numParams;
    for(int i = 0; i < numParams; i++)
        params[i] = m.params[i];
    transparency = m.transparency;
    speedOfLight = m.speedOfLight;
    reflectivity = m.reflectivity;
    specularExponent = m.specularExponent;
    specularFraction = m.specularFraction;
    surfaceRoughness = m.surfaceRoughness;
    ambient.set(m.ambient);
    diffuse.set(m.diffuse);
    specular.set(m.specular);
    emissive.set(m.emissive);
}

/* Shape methods are here */
void Shape::tellMaterialsGL() {
    float amb[4],diff[4],spec[4], emiss[4];
    mat.ambient.build4tuple(amb); // fill the array
    mat.diffuse.build4tuple(diff);
    mat.specular.build4tuple(spec);
    mat.emissive.build4tuple(emiss);
    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMaterialfv(GL_FRONT, GL_EMISSION, emiss);
    glMaterialf(GL_FRONT, GL_SHININESS, mat.specularExponent);
}

void Shape::xfrmRay(Ray& genRay, Ray& r) {
    float v[4];
    r.start.build4tuple(v);
    invTransf.applyTransform(v);
    genRay.setStart(Point3(v[0], v[1], v[2]));
    r.dir.build4tuple(v);
    invTransf.applyTransform(v);
    genRay.setDir(Vector3(v[0], v[1], v[2]));
}

/* TaperedCylinder methods are here */
void TaperedCylinder::drawOpenGL() {
    tellMaterialsGL();
    glPushMatrix();
    glMultMatrixf(transf.matrix);
    glEnable(GL_NORMALIZE);
    glBindTexture(GL_TEXTURE_2D, pixmap[0].textureName);
    gluCylinder(pwall, 1.0, smallRadius,
                1.0, 30, 4); // Draw the cylinder wall
    glPushMatrix();
    glScalef(1, 1, -1); // Flip base so normals point outside
    gluDisk(pbase, 0, 1, 30, 8); // Draw base at z = 0
    glPopMatrix();
    glTranslatef(0, 0, 1);
    gluDisk(pcap, 0, smallRadius, 30, 8); // Draw cap at z = 1
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool TaperedCylinder::hit(Ray& r, Intersection& inter) {
    Ray genRay;
    xfrmRay(genRay, r);
    double A, B, C, F, d, t1, t2;
    Vector3 dir = genRay.dir;
    Point3 point = genRay.start;
    F = 1 + (smallRadius - 1)*point.z;
    d = (smallRadius - 1)*dir.z;
    A = dir.x*dir.x + dir.y*dir.y - d*d;
    B = point.x*dir.x + point.y*dir.y - F*d;
    C = point.x*point.x + point.y*point.y - F*F;
    double discrim = B*B - A*C;
    int num_hits = 0;
    if (discrim > 0.0) {
        double discRoot = sqrt(discrim);
        t1 = (-B - discRoot)/(A);
        Point3 P(genRay.rayPos(t1));
        if (t1 > .00001 && P.z >= 0 && P.z <= 1) {
            inter.hit[num_hits].hitTime = t1;
            inter.hit[num_hits].surface = 0;
            num_hits++;
        }
        t2 = (-B + discRoot)/(A);
        P.set(genRay.rayPos(t2));
        if (t2 > .00001 && P.z >= 0 && P.z <= 1) {
            inter.hit[num_hits].hitTime = t2;
            inter.hit[num_hits].surface = 0;
            num_hits++;
        }
    }

    double tb = -point.z/dir.z;
    if (tb > .00001 &&
        (pow(point.x+dir.x*tb, 2) + pow(point.y+dir.y*tb, 2)) < 1) {
        inter.hit[num_hits].hitTime = tb;
        inter.hit[num_hits].surface = 1;
        num_hits++;
    }
    double tc = (1- point.z)/dir.z;
    if (tc > .00001 &&
        (pow(point.x+dir.x*tc, 2) + pow(point.y+dir.y*tc, 2)) < pow(smallRadius, 2)) {
        inter.hit[num_hits].hitTime = tc;
        inter.hit[num_hits].surface = 2;
        num_hits++;
    }

    if (num_hits == 0)
        return false;

    if (num_hits == 1) { // We were inside the cylinder!
        inter.hit[0].isEntering = false;
        inter.hit[0].hitObject = this;
    } else { // There were two hits
        // Check if the hits are swapped;
        if (inter.hit[0].hitTime > inter.hit[1].hitTime) {
            int tempSurface = inter.hit[0].surface;
            double tempTime = inter.hit[0].hitTime;
            inter.hit[0].hitTime = inter.hit[1].hitTime;
            inter.hit[0].surface = inter.hit[1].surface;

            inter.hit[1].hitTime = tempTime;
            inter.hit[1].surface = tempSurface;
        }
        inter.hit[0].isEntering = true;
        inter.hit[1].isEntering = false;
        inter.hit[0].hitObject = inter.hit[1].hitObject = this;
    }

    for (int i = 0; i < num_hits; i++) {
        Point3 hit(genRay.rayPos(inter.hit[i].hitTime));
        Vector3 norm;
        inter.hit[i].hitPoint.set(hit);
        switch(inter.hit[i].surface) {
        case 0: {
            norm.set(hit.x, hit.y, -(smallRadius-1)*(1+(smallRadius-1)*hit.z));
            inter.hit[i].hitNormal.set(norm);
            break;
        }
        case 1:
            inter.hit[i].hitNormal.set(0,0,-1);
            break;
        case 2:
            inter.hit[i].hitNormal.set(0,0,1);
            break;
        }
    }
    inter.numHits = num_hits;
    return true;
}

void Cube::drawOpenGL() {
    tellMaterialsGL();
    glPushMatrix();
    glMultMatrixf(transf.matrix); // load affine
    glEnable(GL_NORMALIZE);

    if (textureExists) {
        // Front Face
        if (pixmap[0].exists) {
            glBindTexture(GL_TEXTURE_2D, pixmap[0].textureName);
        }
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        // Back Face
        if (pixmap[1].exists) {
            glBindTexture(GL_TEXTURE_2D, pixmap[1].textureName);
        }
        glBegin(GL_QUADS);

        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        // Top Face
        if (pixmap[2].exists) {
            glBindTexture(GL_TEXTURE_2D, pixmap[2].textureName);
        }
        glBegin(GL_QUADS);

        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        // Bottom Face
        if (pixmap[3].exists) {
            glBindTexture(GL_TEXTURE_2D, pixmap[3].textureName);
        }
        glBegin(GL_QUADS);

        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        // Right face
        if (pixmap[4].exists) {
            glBindTexture(GL_TEXTURE_2D, pixmap[4].textureName);
        }
        glBegin(GL_QUADS);

        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        // Left Face
        if (pixmap[5].exists) {
            glBindTexture(GL_TEXTURE_2D, pixmap[5].textureName);
        }
        glBegin(GL_QUADS);

        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
    } else {
        glutSolidCube(2.0);
    }
    glPopMatrix();
}

bool Cube::hit(Ray& r, Intersection& inter) {
    double tHit, numer, denom;
    double tIn = -100000.0, tOut = 100000.0;
    int inSurf, outSurf;
    Ray genRay;
    xfrmRay(genRay, r);
    Point3 point(genRay.start);
    Vector3 dir(genRay.dir);

    for (int i = 0; i < 6; i++) {
        switch(i) {
        case 0:
            numer = 1.0 - point.y;
            denom = dir.y;
            break;
        case 1:
            numer = 1.0 + point.y;
            denom = -dir.y;
            break;
        case 2:
            numer = 1.0 - point.x;
            denom = dir.x;
            break;
        case 3:
            numer = 1.0 + point.x;
            denom = -dir.x;
            break;
        case 4:
            numer = 1.0 - point.z;
            denom = dir.z;
            break;
        case 5:
            numer = 1.0 + point.z;
            denom = -dir.z;
            break;
        }
        if (fabs(denom) < 0.00001) {
            if (numer < 0)
                return false;
        } else {
            tHit = numer/denom;
            if (denom > 0) {
                if (tHit < tOut) {
                    tOut = tHit;
                    outSurf = i;
                }
            } else {
                if (tHit > tIn) {
                    tIn = tHit;
                    inSurf = i;
                }
            }
        }
        if (tIn >= tOut)
            return false;
    }
    int num = 0;
    if (tIn > 0.00001) {
        inter.hit[0].hitTime = tIn;
        inter.hit[0].surface = inSurf;
        inter.hit[0].isEntering = true;
        inter.hit[0].hitObject = this;
        inter.hit[0].hitPoint.set(genRay.rayPos(tIn));
        inter.hit[0].hitNormal.set(cubeNormal(inSurf));
        num++;
    }
    if (tOut > 0.00001) {
        inter.hit[num].hitTime = tOut;
        inter.hit[num].surface = outSurf;
        inter.hit[num].isEntering = false;
        inter.hit[num].hitObject = this;
        inter.hit[num].hitPoint.set(genRay.rayPos(tOut));
        inter.hit[num].hitNormal.set(cubeNormal(outSurf));
        num++;
    }
    inter.numHits = num;
    return (num > 0);
}

Vector3 Cube::cubeNormal(int which) {
    Vector3 norm;
    switch(which) {
    case 0:
        norm.set(0,1,0);
        break;
    case 1:
        norm.set(0,-1,0);
        break;
    case 2:
        norm.set(1,0,0);
        break;
    case 3:
        norm.set(-1,0,0);
        break;
    case 4:
        norm.set(0,0,1);
        break;
    case 5:
        norm.set(0,0,-1);
        break;
    }
    return norm;
}

/* Square methods are here */
void Square::drawOpenGL() {
    tellMaterialsGL();
    glPushMatrix();
    glMultMatrixf(transf.matrix); // load affine
    glEnable(GL_NORMALIZE);
    glBegin(GL_POLYGON);
    glVertex3f(1.0, 1.0, 0);
    glVertex3f(-1.0, 1.0, 0);
    glVertex3f(-1.0, -1.0, 0);
    glVertex3f(1.0, -1.0, 0);
    glEnd();
    glPopMatrix();
}

bool Square::hit(Ray& r, Intersection& inter) {
    Ray genRay;
    inter.numHits = 0;
    xfrmRay(genRay, r);
    double denom = genRay.dir.z;
    if (fabs(denom) < 0.0001)
        return false;
    double time = -genRay.start.z/denom;
    if (time < 0)
        return false;
    double hitx = genRay.start.x + genRay.dir.x*time;
    double hity = genRay.start.y + genRay.dir.y*time;
    if ((hitx > 1.0) || (hitx < -1.0))
        return false;
    if ((hity > 1.0) || (hity < -1.0))
        return false;
    inter.numHits = 1;
    inter.hit[0].hitObject = this;
    inter.hit[0].hitTime = time;
    inter.hit[0].isEntering = true;
    inter.hit[0].surface = 0;
    inter.hit[0].hitPoint.set(hitx, hity, 0);
    inter.hit[0].hitNormal.set(0,0,1);
    return true;
}

/* Sphere methods are here */
void Sphere::drawOpenGL() {
    tellMaterialsGL();
    glPushMatrix();
    glMultMatrixf(transf.matrix);
    if (textureExists) {
        GLUquadricObj *sphere = NULL;
        sphere = gluNewQuadric();
        glPushMatrix();
        //gluQuadricDrawStyle(sphere, GLU_FILL);
        gluQuadricTexture(sphere, GL_TRUE);
        //gluQuadricNormals(sphere, GLU_SMOOTH);
        glBindTexture(GL_TEXTURE_2D, pixmap[0].textureName);
        gluSphere(sphere, 1.0, 20, 20);
        gluDeleteQuadric(sphere);
        glBindTexture(GL_TEXTURE_2D, 0);
        glPopMatrix();
    } else {
        GLUquadricObj *sphere = NULL;
        sphere = gluNewQuadric();
        gluSphere(sphere, 1, 20, 20);
        gluDeleteQuadric(sphere);
    }
    glPopMatrix();
}

bool Sphere::hit(Ray& r, Intersection& inter) {
    Ray genRay;
    xfrmRay(genRay, r);
    double A, B, C;
    A = genRay.dir.dot(genRay.dir);
    if (!(A > 0))
        return false;
    B = dot3(genRay.start, genRay.dir);
    C = dot3(genRay.start, genRay.start) - 1.0;
    double discrim = B*B - A*C;
    if (discrim < 0.0) // Ray missed
        return false;
    int num_hits = 0; // Number of hits
    double discRoot = sqrt(discrim);
    double t1 = (-B - discRoot)/(A);
    if (t1 > .00001) {
        inter.hit[0].hitTime = t1;
        inter.hit[0].hitObject = this;
        inter.hit[0].isEntering = true;
        inter.hit[0].surface = 0;
        Point3 P(genRay.rayPos(t1)); // Hit Position
        inter.hit[0].hitPoint.set(P);
        inter.hit[0].hitNormal.set(P);
        num_hits = 1;
    }
    double t2 = (-B + discRoot)/A;
    if (t1 > .00001) {
        inter.hit[num_hits].hitTime = t2;
        inter.hit[num_hits].hitObject = this;
        inter.hit[num_hits].isEntering = false;
        inter.hit[num_hits].surface = 0;
        Point3 P(genRay.rayPos(t2)); // Hit Position
        inter.hit[num_hits].hitPoint.set(P);
        inter.hit[num_hits].hitNormal.set(P);
        num_hits++;
    }
    inter.numHits = num_hits;
    return (num_hits > 0);
}
