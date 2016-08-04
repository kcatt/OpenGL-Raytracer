#include "pixmap.h"
#include <iostream>
#include <string>

using namespace std;

typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned char Uint8;

typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef long LONG;

typedef struct BITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG  biWidth;
    LONG  biHeight;
    WORD  biPlanes;
    WORD  biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG  biXPelsPerMeter;
    LONG  biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

fstream infile;

ushort getShort() {
    char ic;
    ushort ip;
    infile.get(ic);
    ip = ic;
    infile.get(ic);
    ip |= ((ushort)ic << 8);
    return ip;
}

ulong getLong() {
    ulong ip = 0;
    char ic = 0;
    unsigned char uc = ic;
    infile.get(ic);
    uc = ic;
    ip = uc;
    infile.get(ic);
    uc = ic;
    ip |= ((ulong)uc << 8);
    infile.get(ic);
    uc = ic;
    ip |= ((ulong)uc << 16);
    infile.get(ic);
    uc = ic;
    ip |= ((ulong)uc << 24);
    return ip;
}

bool Pixmap::readBMPFile(string name, GLuint & texture) {
    /*
      infile.open(name.c_str(), ios::in|ios::binary);
      if (!infile) {
      cerr << "Can't open file: " << name << endl;
      return false;
      }
      int k, row, col, numPadBytes, nBytesInRow;
      char ch1, ch2;
      infile.get(ch1);
      infile.get(ch2);
      ulong fileSize = getLong();
      ushort reserved1 = getShort();
      ushort reserved2 = getShort();
      ulong offBits = getLong();
      ulong headerSize = getLong();
      ulong numCols = getLong();
      ulong numRows = getLong();
      ushort planes = getShort();
      ushort bitsPerPixel = getShort();
      ulong compression = getLong();
      ulong imageSize = getLong();
      ulong xPels = getLong();
      ulong yPels = getLong();
      ulong numLUTentries = getLong();
      ulong impColors = getLong();
      if (bitsPerPixel != 24) {
      cerr << "Not a 24-bit pixel image!" << endl;
      infile.close();
      return false;
      }
      nBytesInRow = ((3 * numCols+ 3)/4) * 4;
      numPadBytes = nBytesInRow - 3 * numCols;
      rows = numRows;
      cols = numCols;
      pixel = new RGB[rows * cols];
      if (!pixel) {
      infile.close();
      return false;
      }
      long count = 0;
      char skip_bytes;
      for (row = 0; row < rows; row++) {
      for (col = 0; col < cols; col++) {
      char r, g, b;
      infile.get(b);
      infile.get(g);
      infile.get(r);
      pixel[count].r = r;
      pixel[count].g = g;
      pixel[count].b = b;
      count++;
      }
      for (k = 0; k < numPadBytes; k++) {
      infile >> skip_bytes;
      }
      }
      infile.close();
      return true;
    */
    Uint8* datBuff[2] = {NULL, NULL};
    Uint8* pixels = NULL;
    
    BITMAPFILEHEADER* bmpHeader = NULL;
    BITMAPINFOHEADER* bmpInfo = NULL;
    
    ifstream file(name.c_str(), std::ios::binary);
    if (!file) {
	cout << "Failed to open bitmap file" << endl;
	return false;
    }
    datBuff[0] = new Uint8[sizeof(BITMAPFILEHEADER)];
    datBuff[1] = new Uint8[sizeof(BITMAPINFOHEADER)];
    
    file.read((char*)datBuff[0], sizeof(BITMAPFILEHEADER));
    file.read((char*)datBuff[1], sizeof(BITMAPINFOHEADER));
    
    bmpHeader = (BITMAPFILEHEADER*) datBuff[0];
    bmpInfo = (BITMAPINFOHEADER*) datBuff[1];

    if(bmpHeader->bfType != 0x4D42)
	{
	    std::cout << "File \"" << name << "\" isn't a bitmap file\n";
	    return 2;
	}

    pixels = new Uint8[bmpInfo->biSizeImage];

    // Go to where image data starts, then read in image data
    file.seekg(bmpHeader->bfOffBits);
    file.read((char*)pixels, bmpInfo->biSizeImage);

    Uint8 tmpRGB = 0; // Swap buffer
    for (unsigned long i = 0; i < bmpInfo->biSizeImage; i += 3)
	{
	    tmpRGB        = pixels[i];
	    pixels[i]     = pixels[i + 2];
	    pixels[i + 2] = tmpRGB;
	}

    // Set width and height to the values loaded from the file
    GLuint w = bmpInfo->biWidth;
    GLuint h = bmpInfo->biHeight;

    /*******************GENERATING TEXTURES*******************/
    cout << texture << endl;
    glGenTextures(1, &texture);             // Generate a texture
    glBindTexture(GL_TEXTURE_2D, texture); // Bind that texture temporarily
    cout << texture << endl;
    GLint mode = GL_RGB;                   // Set the mode

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	 
    // Create the texture. We get the offsets from the image, then we use it with the image's
    // pixel data to create it.
    glTexImage2D(GL_TEXTURE_2D, 0, mode, w, h, 0, mode, GL_UNSIGNED_BYTE, pixels);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Output a successful message
    std::cout << "Texture \"" << name << "\" successfully loaded.\n";

    // Delete the two buffers.
    delete[] datBuff[0];
    delete[] datBuff[1];
    delete[] pixels;

    return true; // Return success code 
}
