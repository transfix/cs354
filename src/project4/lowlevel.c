/******************************************************************/
/*         Pixel operations                                       */
/*                                                                */
/* Group Members: <FILL IN>                                       */
/******************************************************************/

#ifdef _WIN32
#include <windows.h>
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "lowlevel.h"

/* constants */

#define RED 0    /* offset to red byte */
#define GREEN 1    /* offset to green byte */
#define BLUE 2    /* offset to blue byte */

/* local data */

/* pointer to image */
GLubyte* canvas;

void initCanvas(int w, int h) {
  int i;

  /* get memory */
  canvas = (GLubyte*) malloc(w*h*3*sizeof(GLubyte));
  /* clear it */
  for (i=0; i<w*h*3; i++) {
    canvas[i]=0xFF;
  }
  width = w; 
  height = h;
}

void drawPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
  if ((x < 0) || (x >= width) || (y < 0) || (y >= height)) return;
  canvas[3*width*(y)+3*(x)+RED] = (char)(r*255);
  canvas[3*width*(y)+3*(x)+GREEN] = (char)(g*255);
  canvas[3*width*(y)+3*(x)+BLUE] = (char)(b*255);
}

void getPixel(GLubyte *img, int x, int y, int w, int h, GLfloat *r, GLfloat *g, GLfloat *b) 
{
  if ((x < 0) || (x >= w) || (y < 0) || (y >= h)) 
    return;
  *r = img[3*w*(y)+3*(x)+RED] / 255.0;
  *g = img[3*w*(y)+3*(x)+GREEN] / 255.0;
  *b = img[3*w*(y)+3*(x)+BLUE] / 255.0;
}

/* draw the canvas array on the screen */
void flushCanvas() {
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glRasterPos3f(0.0,0.0,0.0);
  glDrawPixels(width,height,GL_RGB,GL_UNSIGNED_BYTE,canvas);
}

