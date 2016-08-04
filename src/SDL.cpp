#include "Utilities.h"
#include "SDL.h"
//#include "SOIL.h"

#define TWOPI 6.283185

bool LoadGLTextures(string name, GLuint & texture) {
    vector<unsigned char> Pixels;

    int width = 0;
    int height = 0;
    short BitsPerPixel = 0;

    fstream hFile(name.c_str(), std::ios::in | std::ios::binary);
    if (!hFile.is_open()) {
        cout << "File " << name << " not found." << endl;
        return false;
    }

    hFile.seekg(0, std::ios::end);
    int Length = hFile.tellg();
    hFile.seekg(0, std::ios::beg);
    vector<uint8_t> FileInfo(Length);
    hFile.read(reinterpret_cast<char*>(FileInfo.data()), 54);

    if(FileInfo[0] != 'B' && FileInfo[1] != 'M')
    {
        hFile.close();
        cout << "Invalid file - Must be a bmp file." << endl;
        return false;
    }

    if (FileInfo[28] != 24 && FileInfo[28] != 32)
    {
        hFile.close();
        cout << "Invalid file - Must be a 24 or 32 bit image." << endl;
        return false;
    }

    BitsPerPixel = FileInfo[28];
    width = FileInfo[18] + (FileInfo[19] << 8);
    height = FileInfo[22] + (FileInfo[23] << 8);
    uint32_t PixelsOffset = FileInfo[10] + (FileInfo[11] << 8);
    uint32_t size = ((width * BitsPerPixel + 31) / 32) * 4 * height;
    Pixels.resize(size);

    hFile.seekg (PixelsOffset, std::ios::beg);
    hFile.read(reinterpret_cast<char*>(Pixels.data()), size);
    hFile.close();

    glGenTextures(1, &texture);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, texture);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, Pixels.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    return true;
}
/*
double dot3(Point3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

double dot3(Point3 a, Point3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
    }*/

void checkOrthogVects(Vector3 a, Vector3 b, Vector3 c) {
    // check that system is orthonormal
    double aDotb = fabs(a.dot(b));
    double aDotc = fabs(a.dot(c));
    double bDotc = fabs(b.dot(c));
    if( aDotb > 0.000001 || aDotc > 0.000001 || bDotc > 0.000001)
        cout << " Bad!! vectors NOT orthogonal!\n";
}

void xfrmNormal(Vector3& normal, Affine4& inv, Vector3& v) {
    Affine4 temp;
    inv.transpose(temp);
    float a[4];
    v.build4tuple(a);
    temp.applyTransform(a);
    normal.set(a[0],a[1],a[2]);
}

Point3 rayPos(Ray& ray, double t) {
    Vector3 temp(ray.dir*t);
    Point3 result(ray.start, temp.x, temp.y, temp.z);
    return result;
}

#define SIZESQ(n) ((n.x)*(n.x)+(n.y)*(n.y)+(n.z)*(n.z))

// Creates the normal of the polygon using the four vertices
Vector3 newell4Pts(Point3 a, Point3 b, Point3 c, Point3 d) {
    Vector3 m;
    m.x = (a.y-b.y)*(a.z+b.z)+(b.y-c.y)*(b.z+c.z)+
        (c.y-d.y)*(c.z+d.z)+(d.y-a.y)*(d.z+a.z);
    m.y = (a.z-b.z)*(a.x+b.x)+(b.z-c.z)*(b.x+c.x)+
        (c.z-d.z)*(c.x+d.x)+(d.z-a.z)*(d.x+a.x);
    m.z = (a.x-b.x)*(a.y+b.y)+(b.x-c.x)*(b.y+c.y)+
        (c.x-d.x)*(c.y+d.y)+(d.x-a.x)*(d.y+a.y);
    return m;
}

/* DefUnitStack methods are here */
void DefUnitStack::push (const string & n, const string & s) {
    DefUnit temp (n, s);
    defStack.insert (defStack.begin(), temp);
}

void DefUnitStack::print() {
    for (int i = 0; i < defStack.size(); i++)
        cout << defStack[i].name << ":" << defStack[i].stuff << endl;
}

bool DefUnitStack::search (const string & s) {
    for (int i = 0; i < defStack.size(); i++) {
        if (defStack[i].name == s)
            return true;
    }
    return false;
}

string DefUnitStack::contents (const string & s) {
    for (int i = 0; i < defStack.size(); i++) {
        if (defStack[i].name == s)
            return defStack[i].stuff;
    }
    return NULL;
}

/* Scene methods are here */
string Scene :: nexttoken(void) {
    char c;
    string token;
    int lastchar = 1;
    // If the infile is not existent
    if (!f_in) {
        return token;
    }
    // If the infile is at end of file
    if (f_in->eof()) {
        return token;
    }
    // While there is another character
    while (f_in->get(c)) {
        if (f_in->eof()) {
            return token;
        }
        switch (c) {
        case '\n':
            nextline++;
            // If some form of whitespace
        case ' ' :
        case '\t':
        case '\a':
        case '\b':
        case '\v':
        case '\f':
        case '\r': {
            if ( lastchar == 0 ) {
                return token;
            }
            break;
        }
        case '{': {
            token = c;
            return token;
            break;
        }
        case '}': {
            token = c;
            return token;
            break; }
        case '!': {
            // This is a comment. Read the rest of the line and discard
            while ( c != '\n' && f_in->get(c));
            nextline++;
            break;
        }
        default: {
            token = token + c;
            lastchar = 0;

            if ((f_in->peek() == '{') ||
                (f_in->peek() == '}')) {
                if ( lastchar == 0 ) {
                    return token;
                } else {
                    f_in->get(c);
                    token = c;
                    return token;
                }
            }
            line = nextline;
        }
        }
    }
    return(" ");
}

// Empty the vectors that hold the scene
void Scene :: freeScene() {
    obj.clear();
    light.clear();
    affStk.tos.clear();
}

// Get the number that corresponds to certain keywords
float Scene :: getFloat() {
    stringstream tmp;
    float number;
    string str = nexttoken();
    tmp << str;
    if(!(tmp >> number)) {
        cerr << "Line " << line << ": error getting float" << endl;
        exit(-1);
    } else {
        char t;
        // If the input is succesful there is a character in the number
        if ((tmp >> t )) {
            cerr << "Line " << line << ": bad chars in number" << endl;
            exit(-1);
        }
    }
    return number;
}

// If the keyword is an identifier returns true
bool Scene :: isidentifier(string keyword) {
    string temp = keyword;
    if (!isalpha(temp[0]))
        return false;
    for (int count = 1; count < temp.length(); count++) {
        if ((!isalnum(temp[count]))&& (temp[count] != '.'))
            return false;
    }
    return true;
}

// Release after parsing the file
void Scene :: cleanUp() {
    affStk.releaseAffines();
    delete def_stack; // Release the DefUnitStack memory
}

// Read in the scene and return true if it works
bool Scene:: read(string fname) {
    file_in = new ifstream(fname.c_str());
    if(!(*file_in)) {
        cout << "I can't find or open file: " << fname << endl;
        return false;
    }
    f_in = new stringstream();
    line = nextline = 1;
    def_stack = new DefUnitStack();
    char ch;
    freeScene(); // Delete any previous scene

    obj.clear();
    light.clear();
    affStk.tos.clear();
    AffineNode temp;
    affStk.tos.push_back(temp);

    // Read the whole file
    while (file_in->get(ch)) {
        *f_in << ch;
    }
    // Read the file collecting the objects until an EOF or error is found
    while(getObject());
    file_in->close();
    cleanUp(); // delete temp lists, etc.

    return true;
}

// Make the lights for displaying
void Scene:: makeLightsOpenGL() {
    int lightNum = GL_LIGHT0;
    float colr[4], posn[4]; // arrays to hold openGL color and position
    for (int i = 0; i < light.size() && i < 8; i++, lightNum++) {
        glEnable(lightNum);
        (light[i].color).build4tuple(colr);
        (light[i].pos).build4tuple(posn);
        glLightfv(lightNum,GL_DIFFUSE,colr);
        glLightfv(lightNum,GL_SPECULAR,colr);
        glLightfv(lightNum,GL_POSITION,posn);
    }
}

// Return the token encountered in the scene file
mTokenType Scene :: whichtoken(string keyword) {
    string temp = keyword;
    if (temp == "makePixmap")
        return MAKEPIXMAP;
    if (temp == "light" )
        return LIGHT;
    if (temp == "rotate" )
        return ROTATE;
    if (temp == "translate" )
        return TRANSLATE;
    if (temp == "scale")
        return SCALE;
    if (temp == "push")
        return PUSH;
    if (temp == "pop")
        return POP;
    if (temp == "identityAffine")
        return IDENTITYAFFINE;
    if (temp == "cube")
        return CUBE;
    if (temp == "sphere")
        return SPHERE;
    if (temp == "torus")
        return TORUS;
    if (temp == "plane")
        return PLANE;
    if (temp == "taperedCylinder")
        return TAPEREDCYLINDER;
    if (temp == "teapot")
        return TEAPOT;
    if (temp == "mesh")
        return MESH;
    if (temp == "defaultMaterials")
        return DEFAULTMATERIALS;
    if (temp == "ambient")
        return AMBIENT;
    if (temp == "diffuse")
        return DIFFUSE;
    if (temp == "specular")
        return SPECULAR;
    if (temp == "specularFraction")
        return SPECULARFRACTION;
    if (temp == "surfaceRoughness")
        return SURFACEROUGHNESS;
    if (temp == "emissive")
        return EMISSIVE;
    if (temp == "specularExponent")
        return SPECULAREXPONENT;
    if (temp == "speedOfLight")
        return SPEEDOFLIGHT;
    if (temp == "transparency")
        return TRANSPARENCY;
    if (temp == "reflectivity")
        return REFLECTIVITY;
    if (temp == "parameters")
        return PARAMETERS;
    if (temp == "texture")
        return TEXTURE;
    if (temp == "globalAmbient")
        return GLOBALAMBIENT;
    if (temp == "minReflectivity")
        return MINREFLECTIVITY;
    if (temp == "minTransparency")
        return MINTRANSPARENCY;
    if (temp == "maxRecursionDepth")
        return MAXRECURSIONDEPTH;
    if (temp == "background")
        return BACKGROUND;
    if (temp == "{")
        return LFTCURLY;
    if (temp == "}")
        return RGHTCURLY;
    if (temp == "def")
        return DEF;
    if (temp == "use")
        return USE;
    if (temp == " ")
        return T_NULL;
    if (temp == "square")
        return SQUARE;
    if (isidentifier(temp))
        return IDENT;
    return UNKNOWN;
}

// Draw each object on the list using polymorphism
void Scene :: drawSceneOpenGL() {
    for (int i = 0; i < obj.size(); i++)
        obj[i]->drawOpenGL();
}

// Reads the tokens from the Scene stream and builds all objects
// and necessary functions. If there is an error or the end of file is
// found, it will return NULL.
bool Scene :: getObject() {
    string s;
    GeomObj *newShape;
    mTokenType typ;
    while ((typ = (whichtoken(s = nexttoken()))) != T_NULL) {
        switch(typ) {
        case MAKEPIXMAP: {
            /*int which = getFloat();
              if (which < 0 || which > 7) {
              cerr << "Bad index of pixmap" << endl;
              }
              string file_name = nexttoken();
              if (!pixmap[which].readBMPFile(file_name)) {
              cerr << "Can't read the BMP file." << endl;
              return NULL;
              }*/
            break;
        }
        case LIGHT: {
            Point3 p;
            Color3 c;
            p.x = getFloat();
            p.y = getFloat();
            p.z = getFloat();
            c.red = getFloat();
            c.green = getFloat();
            c.blue = getFloat();
            Light l;
            l.setPosition(p);
            l.setColor(c);
            light.insert(light.begin(), l);
            break;
        }
        case ROTATE: {
            float angle;
            Vector3 u;
            angle = getFloat();
            u.x = getFloat();
            u.y = getFloat();
            u.z = getFloat();
            affStk.rotate(angle,u);
            break;
        }
        case TRANSLATE: {
            Vector3 d;
            d.x = getFloat();
            d.y = getFloat();
            d.z = getFloat();
            affStk.translate(d);
            break;
        }
        case SCALE: {
            float sx, sy, sz;
            sx = getFloat();
            sy = getFloat();
            sz = getFloat();
            affStk.scale(sx, sy, sz);
            break;
        }
        case PUSH:
            affStk.duplicate();
            break;
        case POP:
            affStk.popAndDrop();
            break;
        case IDENTITYAFFINE:
            affStk.setIdentity();
            break;
        case AMBIENT: {
            float dr, dg, db;
            dr = getFloat();
            dg = getFloat();
            db = getFloat();
            currMtrl.ambient.set(dr,dg,db);
            break;
        }
        case DIFFUSE: {
            float dr,dg,db;
            dr = getFloat();
            dg = getFloat();
            db = getFloat();
            currMtrl.diffuse.set(dr,dg,db);
            break;
        }
        case SPECULAR:{
            float dr,dg,db;
            dr = getFloat();
            dg = getFloat();
            db = getFloat();
            currMtrl.specular.set(dr,dg,db);
            break;
        }
        case EMISSIVE: {
            float dr,dg,db;
            dr = getFloat();
            dg = getFloat();
            db = getFloat();
            currMtrl.emissive.set(dr,dg,db);
            break;
        }
        case PARAMETERS: {
            currMtrl.numParams = (int)getFloat();
            for(int i = 0; i < currMtrl.numParams; i++)
                currMtrl.params[i] = getFloat();
            break;
        }
        case SPECULARFRACTION:
            currMtrl.specularFraction = getFloat();
            break;
        case SURFACEROUGHNESS:
            currMtrl.surfaceRoughness = getFloat();
            break;
        case TEXTURE: {// get type, 0 for none
            int which = getFloat();
            if (which < 0 || which > 7) {
                cerr << "Bad index of pixmap" << endl;
            }
            string file_name = nexttoken();
            if (!LoadGLTextures(file_name, pixmap[which].textureName)) {
                cerr << "Can't read the BMP file." << endl;
                return false;
            }
            pixmap[which].exists = true;
            textureExists = true;
            break;
        }
        case DEFAULTMATERIALS:
            currMtrl.setDefault();
            break;
        case SPEEDOFLIGHT:
            currMtrl.speedOfLight = getFloat();
            break;
        case SPECULAREXPONENT:
            currMtrl.specularExponent = getFloat();
            break;
        case TRANSPARENCY:
            currMtrl.transparency = getFloat();
            break;
        case REFLECTIVITY:
            currMtrl.reflectivity = getFloat();
            break;
        case GLOBALAMBIENT:
            ambient.red = getFloat();
            ambient.green = getFloat();
            ambient.blue = getFloat();
            break;
        case BACKGROUND:
            background.red = getFloat();
            background.green = getFloat();
            background.blue = getFloat();
            break;
        case MINREFLECTIVITY:
            minReflectivity = getFloat();
            break;
        case MINTRANSPARENCY:
            minTransparency = getFloat();
            break;
        case MAXRECURSIONDEPTH:
            maxRecursionDepth = getFloat();
            break;
        case T_NULL:
            break; // The null token represents end-of-file
        case DEF: {
            string name, temp, lb, rb;
            string inp;
            name = nexttoken();
            if ( whichtoken(name) != IDENT ) {
                cout << "Error:  Identifier expected." << endl;
                return false;
            }
            if ( def_stack->search(name) ) {
                cout << line << ": " << name;
                cout << ": attempt to redefine. " << endl;
                return false;
            }
            lb = nexttoken();
            if ( whichtoken(lb) != LFTCURLY ) {
                cout << "Error: { expected." << endl;
                return false;
            }
            while ( whichtoken( temp = nexttoken()) != RGHTCURLY ) {
                //cout << temp << endl;
                inp = inp + temp + " ";
                if (!f_in) {
                    cout << "Error: end of file detected." << endl;
                    return false;
                }
            }
            // Push the contents of the string on the stack.
            def_stack->push(name, inp);
            break;
        }
        case USE: {
            string name;
            name = nexttoken();
            if (whichtoken(name) != IDENT) {
                cout << line << ": " << name;
                cout << ": identifier expected.";
                return false;
            }
            if (!def_stack->search(name)) {
                cout << line << ": " << name;
                cout << ": not defined.";
                return false;
            }
            stringstream *temp_fin = new stringstream;
            *temp_fin << def_stack->contents(name) << " ";
            *temp_fin << f_in->rdbuf();
            delete (f_in);
            f_in = temp_fin;
            break;
        }
        default:  {
            switch(typ) {
            case CUBE:
                newShape = new Cube;
                break;
            case SPHERE:
                newShape = new Sphere;
                break;
            case TORUS:
                newShape = new Torus;
                break;
            case TAPEREDCYLINDER:
                newShape = new TaperedCylinder;
                ((TaperedCylinder*)newShape)->smallRadius = getFloat();
                break;
            case TEAPOT:
                newShape = new Teapot;
                break;
            case SQUARE:
                newShape = new Square;
                break;
            case MESH: {
                string fname = nexttoken();
                newShape = new Mesh(fname);
                break;
            }// end of case: MESH
            default: {
                cerr << "Line " << nextline
                     << ": unknown keyword " << s << endl;
                return false;
            }
            }
            // common things to do to all Shapes
            ((Shape*)newShape)->mat.set(currMtrl);
            // load transform and its inverse
            ((Shape*)newShape)->transf.set(affStk.tos[0].affn);
            ((Shape*)newShape)->invTransf.set(affStk.tos[0].invAffn);
            if (textureExists) {
                ((Shape*)newShape)->textureExists = true;
                for (int i = 0; i < 6; i++) {
                    ((Shape*)newShape)->pixmap[i] = pixmap[i];
                }
            }
            obj.push_back(newShape);
            numPixmaps = 0;
            textureExists = false;
            for (int i = 0; i < 6; i++) {
                if (pixmap[i].exists)
                    pixmap[i].clear();
            }
            return true;
        }
        }
    }
    return false;
}

void Scene::getFirstHit(Ray& ray, Intersection& best) {
    Intersection inter;
    best.numHits = 0;
    for (int i = 0; i < obj.size(); i++) {
        if (!obj[i]->hit(ray,inter))
            continue;
        if (best.numHits == 0 ||
            inter.hit[0].hitTime < best.hit[0].hitTime)
            best.set(inter);
    }
}

Color3 Scene::shade(Ray& theRay) {
    Color3 color;
    Intersection best;
    getFirstHit(theRay, best);
    HitInfo h = best.hit[0];
    if (best.numHits == 0) {
        return background;
    }
    Point3 hitPoint(theRay.rayPos(h.hitTime));
    Shape* obj = (Shape*)h.hitObject;
    color.set(obj->mat.emissive);
    color.add(ambient, obj->mat.ambient);
    Vector3 normal;
    xfrmNormal(normal, obj->invTransf, h.hitNormal);
    normal.normalize();
    lightingHelper(obj->mat, hitPoint, normal, theRay, color);
    return color;
}

void Scene::lightingHelper(Material &mtrl, Point3 &hit, Vector3 &norm, Ray &r, Color3& clr) {
    Ray feeler;
    feeler.start = hit - r.dir*.001;
    Vector3 v(r.dir*-1);
    Vector3 s, h;
    double lambert, phong;
    for(int i = 0; i < light.size(); i++) {
        feeler.dir.setDiff(light[i].pos, feeler.start);
        if (isInShadow(feeler)) {
            continue;
        }
        s.setDiff(light[i].pos, hit);
        s.normalize();
        h = s+v;
        lambert = s.dot(norm);
        if (lambert > 0.0) {
            Color3 diffuseColor;
            diffuseColor.add(light[i].color, mtrl.diffuse);
            diffuseColor = diffuseColor*lambert;
            clr.add(diffuseColor);
        }
        phong = h.dot(norm);
        if (phong > 0.0) {
            Color3 specularColor;
            phong = pow(phong, mtrl.specularExponent);
            specularColor.add(light[i].color, mtrl.specular);
            specularColor = specularColor*phong;
            clr.add(specularColor);
        }
    }
}

bool Scene::isInShadow(Ray& f) {
    Intersection inter;
    for (int i = 0; i < obj.size(); i++) {
        if (obj[i]->hit(f, inter))
            return true;
    }
    return false;
}

void Scene::setBackground(Color3& c) {
    background.set(c);
}
