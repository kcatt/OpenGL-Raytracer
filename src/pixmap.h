/* Contains class for the usage of pixmaps and any classes 
   to help with th usagefor creating */

#ifndef _PIXMAP_H_
#define _PIXMAP_H_

#include <fstream>
#include <cassert>
#include <GL/glut.h>
#include <iostream>


class RGB {
  public:
    int r, g, b;

    RGB() {
      r = g = b = 0;
    }

    RGB(RGB& source) {
      r = source.r;
      g = source.g;
      b = source.b;
    }

    RGB(int rr, int gg, int bb) {
      r = rr;
      g = gg;
      b = bb;
    }

    void set (int rr, int gg, int bb) {
      r = rr;
      g = gg;
      b = bb;
    }
};

class Pixmap {
  private:
    RGB* pixel;
  public:
    GLuint rows, cols;
    GLuint textureName;
    bool exists;
    
    Pixmap() {
      cols = rows = 0;
      pixel = 0;
      exists = false;
    }

    Pixmap(int rows, int cols) {
      this->rows = rows;
      this->cols = cols;
      exists = false;
      // OpenGL likes one-dimensional arrays for some reason
      pixel = new RGB[rows*cols]; 
    }
    bool readBMPFile(std::string name, GLuint & texture);
    void freePixmap() {
      delete[] pixel;
    }

    void draw() {
      if (cols == 0 || rows == 0)
        return;
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glDrawPixels(cols, rows, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    }

    bool read(int x, int y, int width, int height) {
      cols = width;
      rows = height;
      pixel = new RGB[rows*cols];
      if (!pixel)
        return false;

      glPixelStorei(GL_PACK_ALIGNMENT, 1);
      glReadPixels(x, y, cols, rows, GL_RGB, GL_UNSIGNED_BYTE, pixel);
      return true;
    }

    void setPixel (int x, int y, RGB color) {
      if (x >= 0 && x < cols && y >= 0 && y < rows)
        pixel[cols*y + x] = color;
    }

    RGB getPixel(int x, int y) {
      assert (x >= 0 && x < cols);
      assert (y >= 0 && y < rows);
      return pixel[cols*y + x];
    }

    void setTexture(GLuint textureName) {
      glBindTexture(GL_TEXTURE_2D, textureName);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, cols, rows, 0, 
                   GL_RGB, GL_UNSIGNED_BYTE, pixel);
      this->textureName = textureName;
    }

    /*Pixmap & operator=(const Pixmap & source) {
      //std::cout << "operator" << std::endl;
      this->rows = source.rows;
      this->cols = source.cols;
      this->exists = source.exists;
      // OpenGL likes one-dimensional arrays for some reason
      //this->pixel = new RGB[rows*cols];
      for (int i = 0; i < rows*cols; i++) {
        this->pixel[i] = source.pixel[i];
        }
        this->textureName = source.textureName;
      //return *this;
    }*/

    void clear() {
      exists = false;
      freePixmap();
    }
};

#endif
