#include <iostream>
#include <GL/glut.h>
#include <cstdlib>
#include "SDL.h"
#include "Camera.h"
using namespace std;

float WIDTH = 640;
float HEIGHT = 480;
string filename = "scene.txt";

int blockSize = 8;

Scene scn;
Camera cam;

void changeCamera(unsigned char key, int x, int y);
void displaySDL();
void parseOptions(int numArgs, char** arg);

int main (int argc, char** argv) {
    parseOptions(argc, argv);
    glutInit (&argc, argv);

    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow ("SDL Scene");
    scn.read(filename);
    Color3 back(0,0,0);
    glClearColor (back.red, back.green, back.blue, 1);
    scn.setBackground(back);
    glEnable(GL_LIGHTING);
    scn.makeLightsOpenGL();
    glViewport(0,0,WIDTH,HEIGHT);
    cam.set(Point3(0, 1, 15),Point3(0,.5,0),Vector3(0,1,0));
    cam.setShape(30,WIDTH/HEIGHT,.5,50);
    glutDisplayFunc(displaySDL);
    glutKeyboardFunc(changeCamera);

    glShadeModel (GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_2D);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glutMainLoop();

    return 0;

}

void changeCamera (unsigned char key, int x, int y) {
    switch (key) {
    case 'w':
        cam.slide (0,0,-.2);
        break;
    case 's':
        cam.slide (0,0,.2);
        break;
    case 'a':
        cam.slide (-.2,0,0);
        break;
    case 'd':
        cam.slide (.2,0,0);
        break;
    case 'W':
        cam.slide (0,.2,0);
        break;
    case 'S':
        cam.slide (0,-.2,0);
        break;

    case 'P':
        cam.pitch(-1);
        break;
    case 'p':
        cam.pitch(1);
        break;
    case 'Y':
        cam.yaw(-1);
        break;
    case 'y':
        cam.yaw(1);
        break;
    case 'R':
        cam.roll(1);
        break;
    case 'r':
        cam.roll(-1);
        break;
    case 't':
        cam.raytrace(scn, blockSize, WIDTH, HEIGHT);
        break;
    }
    if (key != 't')
        glutPostRedisplay();
}

void displaySDL () {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    scn.drawSceneOpenGL();
    glFlush();
    glutSwapBuffers();
}

void parseOptions(int numArgs, char** arg) {
    vector<string> temp;
    for (int i = 1; i < numArgs; i++) {
        temp.push_back(arg[i]);
    }
    for (int i = 0; i < temp.size(); i+=2) {
        if (temp[i] == "-b" ||
            temp[i] == "-B") {
            if (i+1 == temp.size()) {
                cout << "Blocksize flag set, but no block size given. Defaulting to 8." << endl;
                continue;
            }
            blockSize = atoi(temp[i+1].c_str());
        } else if (temp[i] == "-w" ||
                   temp[i] == "-W") {
            if (i+1 == temp.size()) {
                cout << "Width flag set, but no block size given. Defaulting to 640." << endl;
                continue;
            }
            WIDTH = atoi(temp[i+1].c_str());
        } else if (temp[i] == "-h" ||
                   temp[i] == "-H") {
            if (i+1 == temp.size()) {
                cout << "Height flag set, but no block size given. Defaulting to 480." << endl;
                continue;
            }
            HEIGHT = atoi(temp[i+1].c_str());
        } else if (temp[i] == "-f" ||
                   temp[i] == "-F") {
            if (i+1 == temp.size()) {
              cout << "File flag set, but no filename given. Defaulting to scene.txt." << endl;
              continue;
            }
            filename = temp[i+1];
        }
    }
}
