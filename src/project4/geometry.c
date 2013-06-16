/******************************************************************/
/*         Geometry functions                                     */
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
#include "raytrace.h"

int id = 0;

point* makePoint(GLfloat x, GLfloat y, GLfloat z) {
  point* p;
  /* allocate memory */
  p = (point*) malloc(sizeof(point));
  /* put stuff in it */
  p->x = x; p->y = y; p->z = z; 
  p->w = 1.0;
  return (p);
}

/* makes copy of point (or vector) */
point* copyPoint(point *p0) {
  point* p;
  /* allocate memory */
  p = (point*) malloc(sizeof(point));

  p->x = p0->x;
  p->y = p0->y;
  p->z = p0->z;
  p->w = p0->w;         /* copies over vector or point status */
  return (p);
}

/* unallocates a point */
void freePoint(point *p) {
  if (p != NULL) {
    free(p);
  }
}

/* vector from point p to point q is returned in v */
void calculateDirection(point* p, point* q, vector* v) {
  v->x = q->x - p->x;
  v->y = q->y - p->y;
  v->z = q->z - p->z;
  /* a direction is a point at infinity */
  v->w = 0.0;

  normalize(v);
}

/* given a vector, sets its contents to unit length */
void normalize(vector* v) {
  double r = sqrt((v->x*v->x) + (v->y*v->y) + (v->z*v->z));
  v->x /= r; v->y /= r; v->z /= r;
}

float dotProduct(vector *p, vector *q)
{
  return (p->x*q->x)+(p->y*q->y)+(p->z*q->z);
}

/* point on ray r parameterized by t is returned in p */
void findPointOnRay(ray* r,double t,point* p) {
  p->x = r->start->x + t * r->dir->x;
  p->y = r->start->y + t * r->dir->y;
  p->z = r->start->z + t * r->dir->z;
  p->w = 1.0;
}

/* quadratic stuff */

object *makeQuadratic(GLfloat x, GLfloat y, GLfloat z,GLfloat *coeff)
{
  object *o;
  int i;
  ALLOC_OBJ(o);
  o->type = QUADRATIC;
  o->c = makePoint(x,y,z); /* center */
  for(i=0; i<10; i++)
    o->k[i]=coeff[i];
  o->id = ++id;
  return o;
}

/* returns an object with constants configured for a sphere */
/* f(x,y,z) = x^2/r^2 + y^2/r^2 + z^2/r^2 = 1 */
object *makeSphere(GLfloat x, GLfloat y, GLfloat z, GLfloat r)
{
  object *o;
  ALLOC_OBJ(o);
  o->type = QUADRATIC;
  o->c = makePoint(x,y,z); /* center */
  A(o) = 1/(r*r); B(o) = 1/(r*r); C(o) = 1/(r*r); /* set radius */
  J(o) = -1;
  o->id = ++id;
  return o;
}

/* returns an object with constants configured for an ellipsoid */
/* f(x,y,z) = x^2/a^2 + y^2/b^2 + z^2/c^2 = 1 */
object *makeEllipsoid(GLfloat x, GLfloat y, GLfloat z, GLfloat a, GLfloat b, GLfloat c)
{
  object *o;
  ALLOC_OBJ(o);
  o->type = QUADRATIC;
  o->c = makePoint(x,y,z); /* center */
  A(o) = a==0?0:1/(a*a); B(o) = b==0?0:1/(b*b); C(o) = c==0?0:1/(c*c); /* set dimensions */
  J(o) = -1;
  o->id = ++id;
  return o;
}

/* makes an infinite plane of the form */
/* f(x,y,z) = ax+by+cz-d = 0 */
object *makeInfinitePlane(GLfloat x, GLfloat y, GLfloat z, GLfloat a, GLfloat b, GLfloat c)
{
  object *o;
  ALLOC_OBJ(o);
  o->type = PLANE;
  o->c = makePoint(x,y,z);
  A(o) = a; B(o) = b; C(o) = c;
  D(o) = -a*x-b*y-c*z;
  o->id = ++id;
  return o;
}

/* returns TRUE if ray r hits quadratic surface s, with parameter value in t */
int rayQuadraticIntersect(ray* r,object* s,double* t)
{
  double a,b,c,dx,dy,dz,u,v,w,t1=0,t2=0; /* variable soup */
  double disc; /* discriminant */
  a = r->start->x - s->c->x;
  b = r->start->y - s->c->y;
  c = r->start->z - s->c->z;
  dx = r->dir->x;
  dy = r->dir->y;
  dz = r->dir->z;
  u = A(s)*dx*dx+B(s)*dy*dy+C(s)*dz*dz+D(s)*dx*dy+E(s)*dx*dz+F(s)*dy*dz;
  v = 2*A(s)*a*dx+2*B(s)*b*dy+2*C(s)*c*dz+D(s)*a*dy+D(s)*b*dx+E(s)*a*dz+E(s)*c*dx+
    F(s)*b*dz+F(s)*c*dy+G(s)*dx+H(s)*dy+I(s)*dz;
  w = A(s)*a*a+B(s)*b*b+C(s)*c*c+D(s)*a*b+E(s)*a*c+F(s)*b*c+G(s)*a+H(s)*b+I(s)*c+J(s);
  disc = v*v - 4*u*w;
  if(disc < 0) return FALSE;
  t1 = (-v+sqrt(disc))/(2*u);
  t2 = (-v-sqrt(disc))/(2*u);
  if(t1<0 && t2<0) return FALSE;
  if(t1<t2)
    {
      *t = t1<0?t2:t1;
      return TRUE;
    }
  *t = t2<0?t1:t2;
  return TRUE;
}

/* returns TRUE if ray r hits infinite plane s, with parameter value in t */
int rayPlaneIntersect(ray* r,object* s,double* t)
{
  double a,b,c,dx,dy,dz,t1;
  a = r->start->x; b = r->start->y; c = r->start->z;
  dx = r->dir->x; dy = r->dir->y; dz = r->dir->z;
  t1 = -((A(s)*a+B(s)*b+C(s)*c+D(s))/(A(s)*dx+B(s)*dy+C(s)*dz));
  if(t1<0) return FALSE;
  *t = t1;
  return TRUE;
}

/* calculates the gradient at point p, then normalizes it */
void findQuadraticNormal(ray *r,object *s, point *p, vector *n)
{
  double x,y,z;
  ray tmp;
  vector dir;
  tmp.dir = &dir;

  x = p->x - s->c->x;
  y = p->y - s->c->y;
  z = p->z - s->c->z;
  n->x = 2*A(s)*x+D(s)*y+E(s)*z+G(s);
  n->y = 2*B(s)*y+D(s)*x+F(s)*z+H(s);
  n->z = 2*C(s)*z+E(s)*x+F(s)*y+I(s);
  n->w = 0.0;
  normalize(n);

  /* check whether to return an internal
     normal or an external normal */
  calculateDirection(p,r->start,tmp.dir);
  normalize(tmp.dir);
  if(dotProduct(tmp.dir,n)<0)
    {
      n->x *= -1;
      n->y *= -1;
      n->z *= -1;
    }
}

void findPlaneNormal(ray *r,object *s, point *p, vector *n)
{
  ray tmp;
  vector dir;
  tmp.dir = &dir;

  n->x = A(s);
  n->y = B(s);
  n->z = C(s);
  normalize(n);

  /* check whether to return an internal
     normal or an external normal */
  calculateDirection(p,r->start,tmp.dir);
  normalize(tmp.dir);
  if(dotProduct(tmp.dir,n)<0)
    {
      n->x *= -1;
      n->y *= -1;
      n->z *= -1;
    }
}

void setBoundaries(object *o, int bounded, GLfloat *bounds)
{
  if(bounded & X_BOUNDED)
    {
      o->bounded |= X_BOUNDED;
      o->bounds[0] = bounds[0];
      o->bounds[1] = bounds[1];
    }
  if(bounded & Y_BOUNDED)
    {
      o->bounded |= Y_BOUNDED;
      o->bounds[2] = bounds[2];
      o->bounds[3] = bounds[3];
    }
  if(bounded & Z_BOUNDED)
    {
      o->bounded |= Z_BOUNDED;
      o->bounds[4] = bounds[4];
      o->bounds[5] = bounds[5];
    }
}

/* returns false if the point is outside of the object's boundary */
int checkBounds(object *o, point *p)
{
  int passed = TRUE;
  if(p->w==0.0 || o==NULL) return TRUE; /* return true if the point is invalid */
  if(o->bounded == UNBOUNDED) return TRUE;
  if((o->bounded&X_BOUNDED)&&(p->x<o->bounds[0]||p->x>o->bounds[1])) passed = FALSE;
  if((o->bounded&Y_BOUNDED)&&(p->y<o->bounds[2]||p->y>o->bounds[3])) passed = FALSE;
  if((o->bounded&Z_BOUNDED)&&(p->z<o->bounds[4]||p->z>o->bounds[5])) passed = FALSE;
  return passed;
}
