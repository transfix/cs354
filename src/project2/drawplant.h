#ifndef _DRAWPLANT_H_
#define _DRAWPLANT_H_

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glut.h>

/* how many times the quanta elapses per second */
#define FRAMES 60
#define INTERVAL (1000000/FRAMES)

/* each sub branch will be 90% of the height of it's parent */
#define SCALE 0.9
/* each sub branch will have 60% of the radius of it's parent */
#define RADSCALE 0.6
/* number of units per forward move */
#define FORWARD 20
/* rotate YROTATE degrees about the branch axis for the new branch position */
#define YROTATE 10
/* rotate TILT degrees away from the center branch and the plane of the turtle */
#define TILT 120

#define LSYS_SIZE 5
#define LSYS_DEPTH 5
#define LSYS_DEFAULT 1
/* amount to grow per quantum */
#define GROWTH_RATE .005

/* dimensions of the first branch (trunk) */
#define RADIUS 0.7
#define LENGTH 5.0
#define BASE_TRI 5

void drawPlant(int i,float scale,float radscale,int string);
void time_quantum(int z);
void draw_branch(double height, double radius, int base_tri,GLfloat *color1, GLfloat *color2);
void draw_leaf(double height,double radius,int needles,GLfloat *color);

#endif	/* _DRAWPLANT_H_ */
