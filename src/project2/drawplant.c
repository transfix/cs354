/*
 * drawplant.c
 * -------------
 * Contains the drawing routine and related helper functions for the
 * L-system fractal plant.
 *
 * Group Members: Jose Rivera
 */

#include "drawplant.h"
#include "matrix.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glut.h>

#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

GLfloat deg=0,deg2=0,needle_color=0;
int lsys_depth=0;

float yrotate=YROTATE;
float tilt=TILT;

int j=0;

/* L system:
 * F-move forward FORWARD*SCALE meters.
 * 0,1,2,3,4-draw leaf/sub-branch
 * [,]-push/pop respectively
 * L-draw terminating leaf
 * R-rotate YROTATE degrees around the branch
 * T-tilt (rotate about the z axis of the branch)
 * Turtle starts at (modelx,modely,modelz), is facing upward (y), 
 * and it's plane is parallel with the xy axis
 * Axiom: 0
 * 0 -> FF[RL1][RR2][RRR3]F[RL3][RR1][RRR2]RFLR0
 * 1 -> FL[T[RF]2]R[TRFL]RTFL4
 * 2 -> FL[TRF3]RFLRTFL2
 * 3 -> FL[TFL2RFL]R[T[RFLF3]]RTFL2
 * 4 -> FL[TRFL4]RFLRTFL4
 */
char *lsystem[LSYS_SIZE] = { "FF[RL1][RR2][RRR3]F[RL3][RR1][RRR2]RFLR0",
			     "FL[T[RF]2]R[TRFL]RTFL4",
			     "FL[TRF3]RFLRTFL2",
			     "FL[TFL2RFL]R[T[RFLF3]]RTFL2",
			     "FL[TRFL4]RFLRTFL4" };

int do_growth=0;
double growth=1;

void draw_branch(double height, double radius, int base_tri,GLfloat *color1, GLfloat *color2)
{
  int i;
  GLdouble ymin = 0;
  GLdouble ymax = height; /* orgin is at the center of the base of the cylinder */
  double rad_inc = (2*M_PI)/base_tri;

  glBegin(GL_TRIANGLE_FAN); /* drawing the base of the cylinder */
  glColor3fv(color1);
  glVertex3d(0.0,ymin,0.0);
  glColor3fv(color2);
  for(i=0;i<=base_tri;i++)
      glVertex3d(radius*cos(i*rad_inc),ymin,radius*sin(i*rad_inc));
  glEnd();

  glBegin(GL_TRIANGLE_FAN); /* drawing the top of the cylinder */
  glColor3fv(color1);
  glVertex3d(0.0,ymax,0.0);
  glColor3fv(color2);
  for(i=0;i<=base_tri;i++)
      glVertex3d(radius*cos(i*rad_inc),ymax,radius*sin(i*rad_inc));
  glEnd();

  glBegin(GL_QUAD_STRIP); /* drawing the side of the cylinder */
  for(i=0;i<=base_tri;i++)
    {
      glVertex3d(radius*cos(i*rad_inc),ymax,radius*sin(i*rad_inc));
      glVertex3d(radius*cos(i*rad_inc),ymin,radius*sin(i*rad_inc));
    }
  glEnd();
}

void draw_leaf(double height,double radius,int needles,GLfloat *color)
{
  int i;
  GLdouble base[] = { 0.0,0.0,0.0 };
  double rad_inc = (2*M_PI)/needles;
  glBegin(GL_LINES);
  glColor3fv(color);
  for(i=0; i<needles; i++)
    {
      glVertex3dv(base);
      glVertex3d(radius*cos(i*rad_inc),height,radius*sin(i*rad_inc));
    }
  glEnd();
}

/*
 * Draws the plant.
 */
void drawPlant(int i,float scale,float radscale,int string)
{  
  char *ptr = lsystem[string];
  char ch[] = { '\0','\0' };
  GLfloat c1[] = { 0.6549f,0.4901f,0.2392f }; /* light brown */
  GLfloat c2[] = { 0.3607f,0.2510f,0.2000f }; /* dark brown */
  GLfloat c3[] = { 0.1373f,0.5568f,0.1373f }; /* forest green */

  if(i==0)
    return;
  
  PushMatrix();
  while(*ptr != '\0')
    {
      switch(*ptr)
	{
	case 'F':
	  Rotate(tilt/10000,0.0,0.0,1.0); /* tilt very very slightly */
	  LoadMatrix();
	  if(do_growth && floor(growth)==i)
	    {
	      draw_branch(LENGTH*scale*(growth-1),RADIUS*radscale,BASE_TRI,c1,c2);
	      Translate(0.0,LENGTH*scale*(growth-1),0.0);
	    }
	  else
	    {
	      draw_branch(LENGTH*scale,RADIUS*radscale,BASE_TRI,c1,c2);
	      Translate(0.0,LENGTH*scale,0.0);
	    }
	  break;
	case '[':
	  PushMatrix();
	  break;
	case ']':
	  PopMatrix();
	  break;
	case 'L':
	  if(do_growth && floor(growth)==i)
	      draw_leaf(4*(growth-1)*scale,1*(growth-1)*scale,25,c3);
	  else
	    draw_leaf(4*scale,1*scale,25,c3);
	  break;
	case 'R':
	  Rotate(yrotate,0.0,1.0,0.0);
	  break;
	case 'T':
	  Rotate(tilt,0.0,0.0,1.0);
	  break;
	default:
	  if(isdigit(*ptr))
	    {
	      ch[0] = *ptr;
	      drawPlant(i-1,scale*SCALE,radscale*RADSCALE,atoi(ch));
	    }
	  break;
	}
      ptr++;
    }
  PopMatrix();
}

void time_quantum(int z)
{
  deg = ((int)deg + 1 + rand()%3) % 720;
  deg2 = ((int)deg2 + 1 + rand()%3) % 720;

  /* oscillate the branches gently so as to give the illusion of wind */
  tilt=TILT+sin(deg*0.5*(M_PI/180))/100;
  yrotate=YROTATE+cos(deg2*0.5*(M_PI/180))/1000;

  /* grow the tree according to the GROWTH_RATE */
  if(do_growth)
    {
      growth += GROWTH_RATE;
      if(growth>=2)
	{
	  growth=1;
	  lsys_depth++;
	  if(lsys_depth>LSYS_DEPTH)
	    {
	      lsys_depth=LSYS_DEPTH;
	      do_growth=0; /* stop growing */
	      growth=1;
	      printf("Done growing.\n");
	    }
	  else
	    printf("L-system depth reached %d.\n",lsys_depth);
	}
    }

  glutPostRedisplay();
  glutTimerFunc(INTERVAL/1000,time_quantum,z+1);
}

/* end of drawplant.c */
