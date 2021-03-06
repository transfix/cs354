 /*
 * vrml.c
 * ------
 * Drawing subroutines for each vrml object.
 * Also contains the polygon data for each vrml object.
 *
 * Starter code for Project 1.
 *
 * Group Members: Jose Rivera
 */

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>

#include "vrml.h"

/*
 * Note that in VRML, the face indices have a -1 to represent
 * the termination of a index sequence.
 */

/***********************************************************
 * Begin VRML Cube Data
 ***********************************************************/
GLfloat v_cube_vertices[] = {
	1.632990, 0.000000, 1.154700,
	0.000000, 1.632990, 1.154700,
	-1.632990, 0.000000, 1.154700,
	0.000000, -1.632990, 1.154700,
	0.000000, -1.632990, -1.154700,
	1.632990, 0.000000, -1.154700,
	0.000000, 1.632990, -1.154700,
	-1.632990, 0.000000, -1.154700,
};

GLuint v_cube_indices[] = {
	0, 1, 2, 3, -1,
	4, 5, 0, 3, -1,
	5, 6, 1, 0, -1,
	6, 7, 2, 1, -1,
	3, 2, 7, 4, -1,
	7, 6, 5, 4, -1,
};


/***********************************************************
 * Begin VRML Dodecahedron Data
 ***********************************************************/
GLfloat v_dodeca_vertices[] = {
	1.214120, 0.000000, 1.589310,
	0.375185, 1.154700, 1.589310,
	-0.982247, 0.713644, 1.589310,
	-0.982247, -0.713644, 1.589310,
	0.375185, -1.154700, 1.589310,
	1.964490, 0.000000, 0.375185,
	1.589310, 1.154700, -0.375185,
	0.607062, 1.868350, 0.375185,
	-0.607062, 1.868350, -0.375185,
	-1.589310, 1.154700, 0.375185,
	-1.964490, 0.000000, -0.375185,
	-1.589310, -1.154700, 0.375185,
	-0.607062, -1.868350, -0.375185,
	0.607062, -1.868350, 0.375185,
	1.589310, -1.154700, -0.375185,
	0.982247, 0.713644, -1.589310,
	0.982247, -0.713644, -1.589310,
	-0.375185, 1.154700, -1.589310,
	-1.214120, 0.000000, -1.589310,
	-0.375185, -1.154700, -1.589310,
};

GLuint v_dodeca_indices[] = {
	0, 1, 2, 3, 4, -1,
	0, 5, 6, 7, 1, -1,
	1, 7, 8, 9, 2, -1,
	2, 9, 10, 11, 3, -1,
	3, 11, 12, 13, 4, -1,
	4, 13, 14, 5, 0, -1,
	15, 6, 5, 14, 16, -1,
	17, 8, 7, 6, 15, -1,
	18, 10, 9, 8, 17, -1,
	19, 12, 11, 10, 18, -1,
	16, 14, 13, 12, 19, -1,
	16, 19, 18, 17, 15, -1,
};


/***********************************************************
 * Begin VRML Icosahedron Data
 ***********************************************************/
GLfloat v_icosa_vertices[] = {
	0.552786, 1.701300, 0.894427,
	0.000000, 0.000000, 2.000000,
	1.788850, 0.000000, 0.894427,
	-1.447210, 1.051460, 0.894427,
	-1.447210, -1.051460, 0.894427,
	0.552786, -1.701300, 0.894427,
	1.447210, 1.051460, -0.894427,
	-0.552786, 1.701300, -0.894427,
	-1.788850, 0.000000, -0.894427,
	-0.552786, -1.701300, -0.894427,
	1.447210, -1.051460, -0.894427,
	0.000000, 0.000000, -2.000000,
};

GLuint v_icosa_indices[] = {
	0, 1, 2, -1,
	3, 1, 0, -1,
	4, 1, 3, -1,
	5, 1, 4, -1,
	2, 1, 5, -1,
	0, 2, 6, -1,
	7, 0, 6, -1,
	3, 0, 7, -1,
	8, 3, 7, -1,
	4, 3, 8, -1,
	9, 4, 8, -1,
	5, 4, 9, -1,
	10, 5, 9, -1,
	6, 2, 10, -1,
	2, 5, 10, -1,
	6, 11, 7, -1,
	7, 11, 8, -1,
	8, 11, 9, -1,
	9, 11, 10, -1,
	10, 11, 6, -1,
};


/***********************************************************
 * Begin VRML Pyramid Data
 ***********************************************************/
GLfloat v_pyramid_vertices[] = {
	0.997029, 0.000000, -0.997029,
	0.012175, 1.000000, -0.012175,
	-0.997029, 0.000000, -0.997029,
	-0.012175, 1.000000, -0.012175,
	-0.997029, 0.000000, 0.997029,
	-0.012175, 1.000000, 0.012175,
	0.997029, 0.000000, 0.997029,
	0.012175, 1.000000, 0.012175,
};

GLuint v_pyramid_indices[] = {
	6, 0, 7, -1,
	7, 0, 1, -1,
	0, 2, 1, -1,
	1, 2, 3, -1,
	2, 4, 3, -1,
	3, 4, 5, -1,
	4, 6, 5, -1,
	5, 6, 7, -1,
	4, 0, 6, -1,
	4, 2, 0, -1,
	7, 1, 5, -1,
	1, 3, 5, -1,
};

/***********************************************************
 * Begin GL model data
 ***********************************************************/
GLfloat v_gl_vertices[] = {
  -0.1, 0.5, -0.1, /* 0 */ /* begin back G */
  -0.7, 0.5, -0.1, /* 1 */
  -0.7, -0.5, -0.1, /* 2 */
  -0.1, -0.5, -0.1, /* 3 */
  -0.1, -0.1, -0.1, /* 4 */
  -0.3, -0.1, -0.1, /* 5 */
  -0.3, -0.2, -0.1, /* 6 */
  -0.2, -0.2, -0.1, /* 7 */
  -0.2, -0.3, -0.1, /* 8 */
  -0.5, -0.3, -0.1, /* 9 */
  -0.5, 0.3, -0.1, /* 10 */
  -0.2, 0.3, -0.1, /* 11 */
  -0.2, 0.2, -0.1, /* 12 */
  -0.1, 0.2, -0.1, /* 13 */
  -0.1, 0.5, -0.1, /* 14 */ /* end back */
  -0.1, 0.5, 0.1, /* 15 */ /* begin front G */
  -0.7, 0.5, 0.1, /* 16 */
  -0.7, -0.5, 0.1, /* 17 */
  -0.1, -0.5, 0.1, /* 18 */
  -0.1, -0.1, 0.1, /* 19 */
  -0.3, -0.1, 0.1, /* 20 */
  -0.3, -0.2, 0.1, /* 21 */
  -0.2, -0.2, 0.1, /* 22 */
  -0.2, -0.3, 0.1, /* 23 */
  -0.5, -0.3, 0.1, /* 24 */
  -0.5, 0.3, 0.1, /* 25 */
  -0.2, 0.3, 0.1, /* 26 */
  -0.2, 0.2, 0.1, /* 27 */
  -0.1, 0.2, 0.1, /* 28 */
  -0.1, 0.5, 0.1, /* 29 */ /* end front */
  
  0.1, 0.5, -0.1, /* 30 */ /* begin L back */
  0.3, 0.5, -0.1, /* 31 */
  0.3, -0.3, -0.1, /* 32 */
  0.7, -0.3, -0.1, /* 33 */
  0.7, -0.5, -0.1, /* 34 */
  0.1, -0.5, -0.1, /* 35 */ /* end L back */
  0.1, 0.5, 0.1, /* 36 */ /* begin L front */
  0.3, 0.5, 0.1, /* 37 */
  0.3, -0.3, 0.1, /* 38 */
  0.7, -0.3, 0.1, /* 39 */
  0.7, -0.5, 0.1, /* 40 */
  0.1, -0.5, 0.1, /* 41 */ /* end L front */
  
};

GLuint v_gl_indices[] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, -1, /* begin G */
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, -1,
  0, 15, 28, 13, -1,
  12, 27, 28, 13, -1,
  10, 25, 24, 9, -1,
  8, 23, 22, 7, -1,
  6, 21, 20, 5, -1,
  5, 20, 19, 4, -1,
  4, 19, 18, 3, -1,
  2, 17, 16, 1, -1,
  11, 26, -1, /* end G */
  30, 31, 32, 33, 34, 35, -1, /* begin L */
  36, 37, 38, 39, 40, 41, -1,
  30, 36, 37, 31, -1,
  32, 38, 39, 33, -1,
  34, 40, 41, 35, -1, /* end L */
}; 

void draw_vrml_object(GLfloat *vert,GLuint *ind,int faces,GLenum mode)
{
  int i;
  GLfloat *v = vert;
  GLuint *in = ind;
  glColor3f(1.0f, 1.0f, 0.0f);
  for(i=0; i<faces; i++)
    {
      glBegin(mode);
      while(*in != -1)
	{
	  glVertex3fv(&v[(*in)*3]);
	  in++;
	}
      in++;
      glEnd();
    }
}

void draw_vrml_cube(void)
{
  draw_vrml_object(v_cube_vertices,v_cube_indices,6,GL_LINE_LOOP);
}

void draw_vrml_dodecahedron(void)
{
  draw_vrml_object(v_dodeca_vertices,v_dodeca_indices,12,GL_LINE_LOOP);
}

void draw_vrml_pyramid(void)
{
  draw_vrml_object(v_pyramid_vertices,v_pyramid_indices,12,GL_LINE_LOOP);
}

void draw_vrml_icosahedron(void)
{
  draw_vrml_object(v_icosa_vertices,v_icosa_indices,20,GL_LINE_LOOP);
}

void draw_vrml_gl(void)
{
  draw_vrml_object(v_gl_vertices,v_gl_indices,16,GL_LINE_LOOP);
}

void print_vrml_object(void)
{
  switch(vr_object)
    {
    case VR_CUBE:
      printf("VRML Cube object selected\n");
      break;
    case VR_DODECAHEDRON:
      printf("VRML Dodecahedron object selected\n");
      break;
    case VR_ICOSAHEDRON:
      printf("VRML Icosahedron object selected\n");
      break;
    case VR_PYRAMID:
      printf("VRML Pyramid object selected\n");
      break;
    default:
      printf("Warning: unknown VRML object\n");
      break;
    }
}

/* end of vrml.c */

