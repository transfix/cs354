#ifndef _LOWLEVEL_H_
#define _LOWLEVEL_H_

/* Pixel drawing stuff (like Project 1) */

/* lowlevel drawing funtions */
void initCanvas(int,int);
void drawPixel(int,int, GLfloat,GLfloat,GLfloat);
void getPixel(GLubyte *img, int x, int y, int w, int h, GLfloat *r, GLfloat *g, GLfloat *b);
void flushCanvas(void);

/* global variables accessed */
extern int width;
extern int height;

#endif	/* _LOWLEVEL_H_ */
