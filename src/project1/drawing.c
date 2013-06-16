/*
 * drawing.c
 * ---------
 * Drawing subroutines for each of the various display modes in the canvas.
 * Also contains the quadrilateral information for a cube and the
 * triangulation information for a cone.
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
#include <stdlib.h>
#if 0
#include <sys/time.h>
#endif
#include <math.h>

#include "drawing.h"
#include "vrml.h"

#ifndef M_PI
#define M_PI 3.14159265359
#endif

/***********************************************************
 * Begin Cube Data
 ***********************************************************/
/*
 * Vertices used in the manually rendered cube.  This is cube
 * that is 1 x 1 x 1 centered at the origin.  Each group of
 * 3 numbers is a vertex, specified in Cartesian coordinates.
 */
GLfloat cube_vertices[] = {
    -0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
};

/*
 * The colors of each vertex in the low level cube.  The index
 * into this array corresponds to the index into cube_vert.
 */
GLfloat cube_colors[] = {
    0.5f, 0.5f, 0.5f,
    0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f,
};

/*
 * Indices into cube_vert, groups of 4 can be used to draw quadrilaterals
 * which represents a face of the cube.  For instance, the first
 * quad means that vertices 0, 1, 3, 2 of the cube_vertices[]
 * are used, in that order, to form the first face of the cube.
 *
 * Note that all front facing quads are specified in a counterclockwise order
 * (that is, if you are looking at the front of a quad, the vertices will
 * appear in counterclockwise order).
 */
GLuint cube_indices[] = {
    0, 2, 3, 1,
    2, 6, 7, 3,
    7, 6, 4, 5,
    4, 0, 1, 5,
    1, 3, 7, 5,
    0, 4, 6, 2,
};
/***********************************************************
 * End Cube Data
 ***********************************************************/


/***********************************************************
 * Begin Cone Data
 ***********************************************************/

/*
 * Data for the triangulation of the surface of a cone that is one
 * unit tall has a unit circle for its base.  The base is composed
 * of 8 equally sized triangles, and the lateral surface of the cone
 * is composed of a different set of 8 equally sized triangles.
 *
 * See documentation in the Cube Data section for information on
 * the meaning of the data in each array.
 */

GLfloat cone_vertices[] = {
    1.0, -0.5, 0.0,         /* begin unit circle at y = -0.5 */
    0.707, -0.5, 0.707,
    0.0, -0.5, 1.0,
    -0.707, -0.5, 0.707,
    -1.0, -0.5, 0.0,
    -0.707, -0.5, -0.707,
    0.0, -0.5, -1.0,
    0.707, -0.5, -0.707,    /* end unit circle at y = -0.5 */
    0.0, -0.5, 0.0,         /* center of the base */
    0.0, 0.5, 0.0,          /* top of the cone */
};

GLfloat cone_colors[] = {
    0.0f, 0.0f, 0.5f,
    0.0f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f,
};

/*
 * Each set of 3 indices represent the vertices of a triangle.
 * Each triangle faces to the outside of the cone.  The vertices
 * of these forward facing triangles are specified in
 * counterclockwise order.
 */
GLuint cone_indices[] = {
    0, 1, 8,
    1, 2, 8,
    2, 3, 8,
    3, 4, 8,
    4, 5, 8,
    5, 6, 8,
    6, 7, 8,
    7, 0, 8,
    1, 0, 9,
    2, 1, 9,
    3, 2, 9,
    4, 3, 9,
    5, 4, 9,
    6, 5, 9,
    7, 6, 9,
    0, 7, 9,
};
/***********************************************************
 * End Cone Data
 ***********************************************************/


/* Uses glut to draw a cube */
void draw_cube_glut(void) {
	/* Draw the cube using glut */

	glColor3f(1.0f, 0.0f, 0.0f);
	if (disp_style == DS_SOLID) {
		glutSolidCube(1.0f);
	} else if (disp_style == DS_WIRE) {
		glutWireCube(1.0f);
	}
}

/*
 * Draws a cube using the data arrays at the top of this file.
 * Iteratively draws each quad in the cube.
 */
void draw_cube_quad(void) {
	int num_indices;
	int i;
	int index1, index2, index3, index4;

	num_indices = sizeof(cube_indices) / sizeof(GLuint);

	/*
	 * Loop over all quads that need to be draen.
	 * Step i by 4 because there are 4 vertices per quad.
	 */
	for (i = 0; i < num_indices; i += 4) {
		/*
		 * Find the index into the vertex array.  The value
		 * in the cube_indices array refers to the index
		 * of the ordered triples, not the index for the
		 * actual GLfloats that comprise the cube_vertices array.
		 * Thus, we need to multiple by 3 to get the real index.
		 */
		index1 = cube_indices[i] * 3;
		index2 = cube_indices[i+1] * 3;
		index3 = cube_indices[i+2] * 3;
		index4 = cube_indices[i+3] * 3;

		glBegin(GL_QUADS);

		/* All arguments here are pointers */
		glColor3fv(  &(cube_colors[index1]) );
		glVertex3fv( &(cube_vertices[index1]) );
		glColor3fv(  &(cube_colors[index2]) );
		glVertex3fv( &(cube_vertices[index2]) );
		glColor3fv(  &(cube_colors[index3]) );
		glVertex3fv( &(cube_vertices[index3]) );
		glColor3fv(  &(cube_colors[index4]) );
		glVertex3fv( &(cube_vertices[index4]) );

		glEnd();

	}
}

/*
 * Draws a cube using the data arrays at the top of this file.
 * Uses GL's vertex arrays, index arrays, color arrays, etc.
 */
void draw_cube_quad_arrays(void) {
	int num_indices;

	num_indices = sizeof(cube_indices) / sizeof(GLuint);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
	glColorPointer(3, GL_FLOAT, 0, cube_colors);
	glDrawElements(GL_QUADS, num_indices,
					GL_UNSIGNED_INT, cube_indices);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

/*
 * Uses glut to draw a cone.  Must render in either solid and wire
 * frame modes, based on the value of the variable disp_style.
 */
void draw_cone_glut(void) 
{
  glColor3f(0.0f,0.0f,1.0f);
  if (disp_style == DS_SOLID) 
    {
      glutSolidCone(1.0,1.0,20,20);
    } 
  else if (disp_style == DS_WIRE) 
    {
      glutWireCone(1.0,1.0,20,20);
    }
  
}

/*
 * Draws a cone using the data arrays at the top of this file.
 * Iteratively draws each triangle in the cone.
 */
void draw_cone_tri(void) 
{
  int num_indices;
  int i;
  int index1, index2, index3;
  
  num_indices = sizeof(cone_indices) / sizeof(GLuint);

  for (i = 0; i < num_indices; i += 3)
    {
      index1 = cone_indices[i] * 3;
      index2 = cone_indices[i+1] * 3;
      index3 = cone_indices[i+2] * 3;

      glBegin(GL_TRIANGLES);
      glColor3fv(  &(cone_colors[index1]) );
      glVertex3fv( &(cone_vertices[index1]) );
      glColor3fv(  &(cone_colors[index2]) );
      glVertex3fv( &(cone_vertices[index2]) );
      glColor3fv(  &(cone_colors[index3]) );
      glVertex3fv( &(cone_vertices[index3]) );
      glEnd();
    }
}

/*
 * Draws a cone using the data arrays at the top of this file.
 * Uses GL's vertex arrays, index arrays, color arrays, etc.
 */
void draw_cone_tri_arrays(void) {
  int num_indices;

  num_indices = sizeof(cone_indices) / sizeof(GLuint);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  glVertexPointer(3, GL_FLOAT, 0, cone_vertices);
  glColorPointer(3, GL_FLOAT, 0, cone_colors);
  glDrawElements(GL_TRIANGLES, num_indices,
		 GL_UNSIGNED_INT, cone_indices);
  
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

/*
 * Draws a cone using a calculated triangulation of the cone surface.
 *
 * Args
 * ----
 * The HEIGHT and RADIUS of the cone.
 *
 * BASE_TRI refers to the number of triangles used to represent
 * the base of the cone.  Each of these triangles should share a
 * common vertex, namely, the center of the base.
 *
 * The final triangulation of the cone surface should include
 * exactly 2 * BASE_TRI.
 */
void draw_cone_tri_calc(double height, double radius, int base_tri)
{
  int i;
  /* lets put the origin at the dead center of the cone. */
  GLdouble ymin = -height/2;
  GLdouble ymax = height/2;
  double rad_inc = (2*M_PI)/base_tri;
  
  glBegin(GL_TRIANGLE_FAN); /* drawing top part of cone */
  glColor3f(0.0f,0.0f,1.0f);
  glVertex3d(0.0,ymax,0.0); /* top of the cone */
  glColor3f(0.0f,0.0f,0.5f);
  for(i=0;i<=base_tri;i++)
      glVertex3d(radius*cos(i*rad_inc),ymin,radius*sin(i*rad_inc));
  glEnd();

  glBegin(GL_TRIANGLE_FAN); /* drawing the base of the cone */
  glColor3f(0.0f,0.0f,1.0f);
  glVertex3d(0.0,ymin,0.0);
  glColor3f(0.0f,0.0f,0.5f);
  for(i=0;i<=base_tri;i++)
    glVertex3d(radius*cos(i*rad_inc),ymin,radius*sin(i*rad_inc));
  glEnd();
}

/* Draw the various vrml scenes */
void draw_vrml(void) 
{
  switch(vr_object)
    {
    case VR_CUBE:
      draw_vrml_cube();
      break;
    case VR_DODECAHEDRON:
      draw_vrml_dodecahedron();
      break;
    case VR_ICOSAHEDRON:
      draw_vrml_icosahedron();
      break;
    case VR_PYRAMID:
      draw_vrml_pyramid();
      break;
    default:
      break;
    }
}


/* Draws a freeform scene */
void draw_free_scene(void)
{
	draw_vrml_gl();
	glColor3f(1.0f, 0.0f, 0.0f);
	glPushMatrix();
	glRotatef(deg,1.0f,0.0f,0.0f);
	glTranslatef(0.0f,0.0f,-1.0f);
	glutSolidSphere(0.1,10,10);
	glPopMatrix();
	glPushMatrix();
	glRotatef(deg2,0.0f,1.0f,0.0f);
	glTranslatef(-1.3f,0.0f,0.0f);
	glutSolidSphere(0.1,10,10);
	glPopMatrix();
	glPushMatrix();
	glRotatef(deg3,-1.118f,0.0f,-1.0f);  /* rotate about a vector perpendicular to the radius vec */
	glRotatef(deg2,0.0f,0.0f,1.0f);      /* chaotic orbit */
	glRotatef(deg,1.0f,1.0f,1.0f);
	glTranslatef(1.0f,0.0f,-0.707f);     /* radius = 1.5 */
	glutSolidSphere(0.1,10,10);
	glPopMatrix();
	glPushMatrix();
	glRotatef(deg,0.0f,0.0f,1.0f);
	glTranslatef(1.0f,0.0f,0.0f);
	glutSolidSphere(0.1,10,10);
	glPopMatrix();
}

/* this works but perhaps this is not the best way...? */
/* this is called every time we get a SIGALRM (every INTERVAL microseconds) */
void calc_free_scene_anim(int z)
{
  if(disp_mode != DM_FREE_SCENE) return;

  deg = ((int)deg + 2) % 360; /* lets rotate 2 degrees FRAMES times a second */
  deg2 = ((int)deg2 + 5) % 360; /* 5 degress FRAMES times a second */
  deg3 = ((int)deg3 + 4) % 360; /* 4 degrees FRAMES times a second */
      
  glutPostRedisplay();
  glutTimerFunc(INTERVAL/1000,calc_free_scene_anim,z+1);
}

/* Prints to stdout the current display mode */
void print_disp_mode( void ) {
    switch (disp_mode) {
        case DM_CUBE_GLUT:
            printf("Display Mode: Cube using glut\n");
            break;
        case DM_CUBE_QUAD:
            printf("Display Mode: Cube using quadrilaterals\n");
            break;
        case DM_CUBE_QUAD_ARRAYS:
            printf("Display Mode: Cube using quadrilateral arrays\n");
            break;
        case DM_CONE_GLUT:
            printf("Display Mode: Cone using glut\n");
            break;
        case DM_CONE_TRI:
            printf("Display Mode: Cone using triangles\n");
            break;
        case DM_CONE_TRI_ARRAYS:
            printf("Display Mode: Cone using triangle arrays\n");
            break;
        case DM_CONE_TRI_CALC:
            printf("Display Mode: Cone using calculated triangles\n");
			break;
        case DM_VRML:
            printf("Display Mode: VRML objects\n");
            break;
        case DM_FREE_SCENE:
            printf("Display Mode: Freeform scene\n");
            break;
        default:
			printf("Warning: unknown display mode\n");
            break;
    }
}


/* Prints to stdout the current display style */
void print_disp_style( void ) {
	if (disp_style == DS_SOLID) {
		printf("Display Style: solid (for glut modes only)\n");
	} else if (disp_style == DS_WIRE) {  
		printf("Display Style: wire (for glut modes only)\n");
	} else {
		printf("Warning: unknown display style\n");
	}
}

/* end of drawing.c */

