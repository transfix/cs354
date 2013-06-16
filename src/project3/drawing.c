/*
 * drawing.c
 * -----------
 * Contains the drawing routines and related helper functions for the
 * subdivision surface
 *
 * Group Members: <FILL IN>
 */

#ifdef _WIN32
#include <windows.h>
#endif
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <string.h>

#include "drawing.h"

/* Globals */
int subdiv_v=0; // The user-specified subdivision level, vertical
int subdiv_h=0; // The user-specified subdivision level, horizontal

extern int mode;
extern int solid;
extern int faces;
extern int normals;
extern int shading;
extern GLfloat dpz; /* distance from camera to drawing plane */
extern GLfloat dptop; /* drawing plane top */
extern GLfloat dpbottom; /* drawing plane bottom */
extern GLfloat dpleft; /* drawing plane left */
extern GLfloat dpright; /* drawing plane right */

struct point_t *line=NULL; /* the line drawn on the plane */
struct slice_t *slices=NULL; /* a list of slices that make up the shape */

void drawSurface(void)
{
  struct point_t *cur;
  struct point_t *cur2;
  struct slice_t *cur_slice;
  struct slice_t *cur2_slice;
  struct point_t points[5];
  GLfloat v1[3],v2[3],v3[3];
  double a,b,c;

  switch(mode)
    {
    case 0:
      /* draw the axis of rotation */
      glBegin(GL_LINES);
      glColor3f(0.0,1.0,0.0);
      glVertex3f(0.0,dptop,0);
      glVertex3f(0.0,dpbottom,0);
      glEnd();

      if(line==NULL) break;

      cur=line;
      glPointSize(5);
      glBegin(GL_POINTS);
      glColor3f(0.0,0.0,1.0);
      while(cur!=NULL)
	{
	  glVertex3f(cur->x,cur->y,cur->z);
	  cur = cur->n;
	}
      glEnd();

      cur=line;
      glBegin(GL_LINES);
      glColor3f(1.0,0.0,0.0);
      while(cur->n!=NULL)
	{
	  glVertex3f(cur->x,cur->y,cur->z);
	  glVertex3f(cur->n->x,cur->n->y,cur->n->z);
	  cur = cur->n;
	}
      glEnd();

      break;
    case 1:

      if(slices==NULL) break;
      if(slices->n==NULL) break;

      if(!faces)
	{
	  glDisable(GL_LIGHTING); /* no lights in point mode */
	  glDisable(GL_LIGHT0);
	  glColor3f(1.0,0.0,0.0);
	  cur_slice=slices;
	  while(cur_slice!=NULL)
	    {
	      cur=cur_slice->line;
	      glPointSize(0.05);
	      glBegin(GL_POINTS);
	      while(cur!=NULL)
		{
		  glVertex3f(cur->x,cur->y,cur->z);
		  cur = cur->n;
		}
	      glEnd();
	      cur_slice=cur_slice->n;
	    }

	  break;
	}

      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
      cur_slice = slices;
      cur2_slice = slices->n;
      while(cur_slice!=NULL)
	{
	  cur = cur_slice->line;
	  cur2 = cur2_slice->line;
	  while(cur->n!=NULL)
	    {
	      if(shading)
		{
		  /* just calc more normals and verticies */
		  
		  /* right vertex */
		  add_vec(&(cur->nx),&(cur->n->nx),&(points[0].nx));
		  normalize(&(points[0].nx));
		  sub_vec(&(cur->n->x),&(cur->x),v1);
		  v1[0] /= 2; v1[1] /= 2; v1[2] /= 2;
		  add_vec(&(cur->x),v1,&(points[0].x));
		  
		  /* top vertex */
		  add_vec(&(cur->nx),&(cur2->nx),&(points[1].nx));
		  normalize(&(points[1].nx));
		  sub_vec(&(cur2->x),&(cur->x),v1);
		  v1[0] /= 2; v1[1] /= 2; v1[2] /= 2;
		  add_vec(&(cur->x),v1,&(points[1].x));
		  
		  /* left vertex */
		  add_vec(&(cur2->nx),&(cur2->n->nx),&(points[2].nx));
		  normalize(&(points[2].nx));
		  sub_vec(&(cur2->n->x),&(cur2->x),v1);
		  v1[0] /= 2; v1[1] /= 2; v1[2] /= 2;
		  add_vec(&(cur2->x),v1,&(points[2].x));
		  
		  /* bottom vertex */
		  add_vec(&(cur2->n->nx),&(cur->n->nx),&(points[3].nx));
		  normalize(&(points[3].nx));
		  sub_vec(&(cur->n->x),&(cur2->n->x),v1);
		  v1[0] /= 2; v1[1] /= 2; v1[2] /= 2;
		  add_vec(&(cur2->n->x),v1,&(points[3].x));
		  
		  /* center vertex */
		  add_vec(&(points[0].nx),&(points[1].nx),v1);
		  add_vec(&(points[2].nx),&(points[3].nx),v2);
		  add_vec(v1,v2,&(points[4].nx));
		  normalize(&(points[4].nx));
		  sub_vec(&(points[3].x),&(cur2->n->x),v1);
		  sub_vec(&(points[2].x),&(cur2->n->x),v2);
		  add_vec(v1,v2,v3);
		  normalize(v3);
		  a=sqrt(v1[0]*v1[0]+v1[1]*v1[1]+v1[2]*v1[2]);
		  b=sqrt(v2[0]*v2[0]+v2[1]*v2[1]+v2[2]*v2[2]);
		  c=sqrt(a*a+b*b);
		  v3[0] *= c; v3[1] *= c; v3[2] *= c;
		  add_vec(&(cur2->n->x),v3,&(points[4].x));

		  glColor3f(0.0,0.0,1.0);
 		  glBegin(GL_POLYGON);

		  /* right */
		  glNormal3fv(&(cur->nx));
		  glVertex3fv(&(cur->x));

		  glNormal3fv(&(points[0].nx));
		  glVertex3fv(&(points[0].x));
		  
		  glNormal3fv(&(cur->n->nx));
		  glVertex3fv(&(cur->n->x));
		  
		  /* middle */
		  glNormal3fv(&(points[1].nx));
		  glVertex3fv(&(points[1].x));
		  
		  glNormal3fv(&(points[4].nx));
		  glVertex3fv(&(points[4].x));

		  glNormal3fv(&(points[3].nx));
		  glVertex3fv(&(points[3].x));

		  /* left */
		  glNormal3fv(&(cur2->n->nx));
		  glVertex3fv(&(cur2->n->x));
		  
		  glNormal3fv(&(points[2].nx));
		  glVertex3fv(&(points[2].x));

		  glNormal3fv(&(cur2->nx));
		  glVertex3fv(&(cur2->x));

		  glEnd();
		}
	      else
		{		  
		  glColor3f(0.0,0.0,1.0);
		  glBegin(GL_POLYGON);
		  glNormal3fv(&(cur->nx));
		  glVertex3fv(&(cur->x));
		  
		  glNormal3fv(&(cur->n->nx));
		  glVertex3fv(&(cur->n->x));
		  
		  glNormal3fv(&(cur2->n->nx));
		  glVertex3fv(&(cur2->n->x));
		  
		  glNormal3fv(&(cur2->nx));
		  glVertex3fv(&(cur2->x));
		  glEnd();
		}

	      if(normals)
		{
		  glBegin(GL_LINES);
		  glColor3f(0.0,1.0,0.0);
		  glVertex3f(cur->x,cur->y,cur->z);
		  glVertex3f(cur->x+cur->nx,cur->y+cur->ny,cur->z+cur->nz);

		  glVertex3f(cur->n->x,cur->n->y,cur->n->z);
		  glVertex3f(cur->n->x+cur->n->nx,cur->n->y+cur->n->ny,cur->n->z+cur->n->nz);

		  glVertex3f(cur2->n->x,cur2->n->y,cur2->n->z);
		  glVertex3f(cur2->n->x+cur2->n->nx,cur2->n->y+cur2->n->ny,cur2->n->z+cur2->n->nz);

		  glVertex3f(cur2->x,cur2->y,cur2->z);
		  glVertex3f(cur2->x+cur2->nx,cur2->y+cur2->ny,cur2->z+cur2->nz);
		  glEnd();
		}

	      cur = cur->n;
	      cur2 = cur2->n;
	    }

	  cur_slice = cur_slice->n;
	  cur2_slice = cur2_slice->n != NULL ? cur2_slice->n : slices; /* circle around */
	}

      break;
    }
}

/* adds a point to the line list */
void addPoint(GLfloat x, GLfloat y, GLfloat z)
{
  struct point_t *cur;
  if(line==NULL)
    {
      ALLOC_POINT(line);
      line->x=x; line->y=y; line->z=z;
      return;
    }
  cur=line;
  while(cur->n!=NULL)
      cur = cur->n;
  ALLOC_POINT(cur->n);
  cur = cur->n;
  cur->x=x; cur->y=y; cur->z=z;
}

/* removes the last point in the line list */
void removePoint()
{
  struct point_t *cur=line;
  if(cur==NULL) return;
  if(cur->n==NULL)
    {
      free(cur);
      line=NULL;
      return;
    }
  while(cur->n!=NULL)
    {
      if(cur->n->n==NULL)
	{
	  free(cur->n);
	  cur->n=NULL;
	  return;
	}
      cur = cur->n;
    }
}

void addSlice(struct point_t *slice)
{
  struct slice_t *cur;
  if(slices==NULL)
    {
      ALLOC_SLICE(slices);
      slices->line = slice;
      return;
    }
  cur=slices;
  while(cur->n!=NULL)
      cur = cur->n;
  ALLOC_SLICE(cur->n);
  cur = cur->n;
  cur->line = slice;
}

/* calculates a new point for 4 point subdivision */
struct point_t *calc_point(struct point_t *p0, struct point_t *p1,
			   struct point_t *p2, struct point_t *p3)
{
  struct point_t *ret;
  ALLOC_POINT(ret);
  ret->x = (GLfloat)((-1.0/16.0)*p0->x + (9.0/16.0)*p1->x + (9.0/16.0)*p2->x + (-1.0/16.0)*p3->x);
  ret->y = (GLfloat)((-1.0/16.0)*p0->y + (9.0/16.0)*p1->y + (9.0/16.0)*p2->y + (-1.0/16.0)*p3->y);
  return ret;
}

/* interleaves src into dest (assumes dest has 1 more point than src) */
void interleave(struct point_t *dest, struct point_t *src)
{
  struct point_t *cur,*cur2,*tmp,*tmp2;
  cur=dest;
  cur2=src;
  while(cur2!=NULL)
    {
      tmp = cur->n;
      tmp2 = cur2->n;
      cur->n = cur2;
      cur2->n = tmp;

      cur = tmp;
      cur2 = tmp2;
    }
}

struct point_t *duplicate_slice(struct point_t *slice)
{
  struct point_t *cur = slice;
  struct point_t *new;
  struct point_t *newcur;

  ALLOC_POINT(new);
  newcur = new;
  while(cur!=NULL)
    {
      newcur->x = cur->x;
      newcur->y = cur->y;
      newcur->z = cur->z;

      cur = cur->n;
      if(cur!=NULL)
	ALLOC_POINT(newcur->n);
      newcur = newcur->n;
    }
  return new;
}

void freeSlice(struct point_t *slice)
{
  struct point_t *curp=slice;
  struct point_t *tmp;

  while(curp!=NULL)
    {
      tmp = curp->n;
      free(curp);
      curp = tmp;
    }
}

void freeModel()
{
  struct slice_t *curs=slices;
  struct slice_t *tmp;

  while(curs!=NULL)
    {
      freeSlice(curs->line);
      tmp = curs->n;
      free(curs);
      curs = tmp;
    }

  slices=NULL;
}

void cross_product(GLfloat *v1,GLfloat *v2,GLfloat *v3)
{
  v3[0] = v1[1]*v2[2]-v2[1]*v1[2];
  v3[1] = -(v1[0]*v2[2]-v2[0]*v1[2]);
  v3[2] = v1[0]*v2[1]-v2[0]*v1[1];
}

void normalize(GLfloat *v)
{
  float n = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
  if(v[0] == 0 && v[1] == 0 && v[2] == 0) return;
  v[0] /= n; v[1] /= n; v[2] /= n;
}

void sub_vec(GLfloat *v1, GLfloat *v2, GLfloat *v3)
{
  v3[0] = v1[0] - v2[0];
  v3[1] = v1[1] - v2[1];
  v3[2] = v1[2] - v2[2];
}

void add_vec(GLfloat *v1, GLfloat *v2, GLfloat *v3)
{
  v3[0] = v1[0] + v2[0];
  v3[1] = v1[1] + v2[1];
  v3[2] = v1[2] + v2[2];
}


void recompute_normals() /* assuming there are at least 3 control points and 3 slices */
{
  struct point_t *cur,*cur2,*cur3;
  struct slice_t *cur_slice, *cur2_slice,*cur3_slice;
  GLfloat v1[3],v2[3],v3[3],v4[3];
  GLfloat tmp1[3],tmp2[3];

  if(slices==NULL || slices->n==NULL || slices->n->n == NULL) return;

  cur_slice = slices;
  cur2_slice = slices->n;
  cur3_slice = slices->n->n;

  while(cur_slice != NULL)
    {
      cur = cur_slice->line;
      cur2 = cur2_slice->line;
      cur3 = cur3_slice->line;

      while(cur->n->n!=NULL)
	{
	  if(cur->z == 0 && cur2->z == 0 && cur3->z == 0) /* point on the axis */
	    {
	      sub_vec(&(cur2->n->x),&(cur2->x),tmp1);
	      sub_vec(&(cur->n->x),&(cur2->x),tmp2);
	      cross_product(tmp1,tmp2,v1);

	      sub_vec(&(cur2->n->x),&(cur2->x),tmp1);
	      sub_vec(&(cur3->n->x),&(cur2->x),tmp2);
	      cross_product(tmp2,tmp1,v2);

	      cur2->n->nx = v1[0] + v2[0];
	      cur2->n->ny = v1[1] + v2[1];
	      cur2->n->nz = v1[2] + v2[2];

	      normalize(&(cur2->n->nx));
	    }
	  else /* most points */
	    {
	      sub_vec(&(cur2->x),&(cur2->n->x),tmp1); /* up & right */
	      sub_vec(&(cur->n->x),&(cur2->n->x),tmp2);
	      cross_product(tmp2,tmp1,v1);
	      
	      sub_vec(&(cur2->x),&(cur2->n->x),tmp1); /* up & left */
	      sub_vec(&(cur3->n->x),&(cur2->n->x),tmp2);
	      cross_product(tmp1,tmp2,v2);
	      
	      sub_vec(&(cur2->n->n->x),&(cur2->n->x),tmp1); /* down & left */
	      sub_vec(&(cur3->n->x),&(cur2->n->x),tmp2);
	      cross_product(tmp2,tmp1,v3);
	      
	      sub_vec(&(cur2->n->n->x),&(cur2->n->x),tmp1); /* down & right */
	      sub_vec(&(cur->n->x),&(cur2->n->x),tmp2);
	      cross_product(tmp1,tmp2,v4);
	      
	      cur2->n->nx = v1[0] + v2[0] + v3[0] + v4[0];
	      cur2->n->ny = v1[1] + v2[1] + v3[1] + v4[1];
	      cur2->n->nz = v1[2] + v2[2] + v3[2] + v4[2];
	      
	      normalize(&(cur2->n->nx));
	    }
 
	  cur = cur->n;
	  cur2 = cur2->n;
	  cur3 = cur3->n;
	}

      cur_slice = cur_slice->n;
      cur2_slice = cur2_slice->n != NULL ? cur2_slice->n : slices; /* circle around */
      cur3_slice = cur3_slice->n != NULL ? cur3_slice->n : slices; /* circle around */
    }
}

/* end of drawplant.c */
