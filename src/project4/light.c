/******************************************************************/
/*         Lighting functions                                     */
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
#include "lowlevel.h"

extern light *ambient;
extern light_list *lights;

material *makeMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat amb, 
		       GLfloat diff, GLfloat spec, GLfloat ns, GLfloat refl,GLfloat trans,GLfloat n) 
{
  material* m;
  
  /* allocate memory */
  m = (material*) malloc(sizeof(material));
  /* put stuff in it */
  m->r = r>1?1:r;
  m->g = g>1?1:g;
  m->b = b>1?1:b;
  m->amb = amb>1?1:amb;
  m->diff = diff>1?1:diff;
  m->spec = spec>1?1:spec;
  m->ns = ns>1?ns:1;
  m->refl = refl>1?1:refl;
  m->trans = trans>1?1:trans;
  m->n = n<1?1:n;
  return(m);
}

material *makeTexturedMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat amb, 
			       GLfloat diff, GLfloat spec, GLfloat ns, GLfloat refl,
			       GLfloat trans,GLfloat n,GLubyte *texture,int t_w,int t_h) 
{
  material* m;
  
  /* allocate memory */
  m = (material*) malloc(sizeof(material));
  /* put stuff in it */
  m->r = r>1?1:r;
  m->g = g>1?1:g;
  m->b = b>1?1:b;
  m->amb = amb>1?1:amb;
  m->diff = diff>1?1:diff;
  m->spec = spec>1?1:spec;
  m->ns = ns>1?ns:1;
  m->refl = refl>1?1:refl;
  m->trans = trans>1?1:trans;
  m->n = n<1?1:n;
  m->texture = texture;
  m->t_w = t_w;
  m->t_h = t_h;
  m->textured = TRUE;
  return(m);
}

light *makeLight(GLfloat r, GLfloat g, GLfloat b, point *p, vector *d)
{
  light *l = (light *)malloc(sizeof(light));
  if(l==NULL)
    {
      fprintf(stderr,"makeLight(): Error allocating memory\n");
      exit(-1);
    }
  if(d!=NULL)
    d->w = 0.0; /* make sure what's passed is a vector */
  l->r = r>1?1:r; /* clamp values to 1 */
  l->g = g>1?1:g;
  l->b = b>1?1:b;
  l->p = p;
  l->d = d;
  /* make sure d is normalized */
  if(d!=NULL)
    normalize(l->d);
  return l;
}

/* LIGHTING CALCULATIONS */

/* shade */
/* color of point p with normal vector n and material m returned in c */
/* in is the direction of the incoming ray and d is the recusive depth */
void shade(point* p, vector* n, material* m, vector* in, color* c, int d) {
  point tmp;
  ray r,r_reflect;
  light_list *cur = lights;
  point dir,start;
  double hit_t=0, light_t=0;
  color color_tmp,color_tmp2,color_tmp3;
  vector v;
  vector rv;
  vector ri;
  GLfloat dot;

  tmp.x = tmp.y = tmp.z = tmp.w = 0.0;
  color_tmp.r = color_tmp.g = color_tmp.b = 0.0;
  color_tmp2.r = color_tmp2.g = color_tmp2.b = 0.0;
  color_tmp3.r = color_tmp3.g = color_tmp3.b = 0.0;
  r.dir = &dir;
  r.start = &start;
  r_reflect.start = &start;
  r_reflect.dir = &rv;

  v.x = -in->x; v.y = -in->y; v.z = -in->z;
  /* make sure v is normalized */
  normalize(&v);
  /* calculate reflected ray */
  rv.x = 2*dotProduct(n,&v)* n->x - v.x;
  rv.y = 2*dotProduct(n,&v)* n->y - v.y;
  rv.z = 2*dotProduct(n,&v)* n->z - v.z;
  normalize(&rv);

  /* find ambient component of color */
  if(!m->textured)
    {
      c->r = m->amb * m->r * ambient->r;
      c->g = m->amb * m->g * ambient->g;
      c->b = m->amb * m->b * ambient->b;
    }
  else
    {
      /* just use the y and z values for the texture mapping.  This
	 is a cheap hack but it will work for an infinite plane with
	 a normal parallel to the x axis */
      getPixel(m->texture,abs((int)(p->y*TEX_SCALE))%m->t_w,abs((int)(p->z*TEX_SCALE))%m->t_h,m->t_w,m->t_h,&(c->r),&(c->g),&(c->b));
      c->r += m->amb * ambient->r;
      c->g += m->amb * ambient->g;
      c->b += m->amb * ambient->b;
    }
  
  while(cur!=NULL)
    {
      /* test if we are in shadow */
      calculateDirection(p,cur->l->p,r.dir);
      r.start->x = p->x + EPSILON * r.dir->x;
      r.start->y = p->y + EPSILON * r.dir->y;
      r.start->z = p->z + EPSILON * r.dir->z;
      boundaryHit(&r,&tmp,NULL,NULL);
      //firstHit(&r,&tmp,NULL,NULL,NULL);
      
      if(tmp.w==1.0) /* the ray hit something, but if it hit past the light source, then
			we're still not in shadow, so we must test for that */
	{
	  hit_t = sqrt((tmp.x-r.start->x)*(tmp.x-r.start->x)+
		       (tmp.y-r.start->y)*(tmp.y-r.start->y)+
		       (tmp.z-r.start->z)*(tmp.z-r.start->z));
	}

      light_t = sqrt((cur->l->p->x-r.start->x)*(cur->l->p->x-r.start->x)+
			 (cur->l->p->y-r.start->y)*(cur->l->p->y-r.start->y)+
			 (cur->l->p->z-r.start->z)*(cur->l->p->z-r.start->z));
      if((tmp.w==0.0) || ((hit_t>light_t)&&(light_t>0))) /* we are not in shadow */
	{
	
	  /* test if we are on the invisible side of a surface */
	  if(dotProduct(r.dir,n)>0.0) /* we're not, so calculate the color */
	    {
	      /* find diffuse component */
	      c->r += m->diff*m->r*cur->l->r*dotProduct(r.dir,n);
	      c->g += m->diff*m->g*cur->l->g*dotProduct(r.dir,n);
	      c->b += m->diff*m->b*cur->l->b*dotProduct(r.dir,n);

	      /* find specular component */
	      /* let Ri be the reflection of r.dir (light vector) across the normal N */
	      ri.x = 2*dotProduct(n,&v)* n->x - r.dir->x;
	      ri.y = 2*dotProduct(n,&v)* n->y - r.dir->y;
	      ri.z = 2*dotProduct(n,&v)* n->z - r.dir->z;
	      normalize(&ri);

	      color_tmp.r += m->spec*(GLfloat)pow((dot=dotProduct(&v,&ri))<0?0:dot,m->ns)*cur->l->r;
	      color_tmp.g += m->spec*(GLfloat)pow((dot=dotProduct(&v,&ri))<0?0:dot,m->ns)*cur->l->g;
	      color_tmp.b += m->spec*(GLfloat)pow((dot=dotProduct(&v,&ri))<0?0:dot,m->ns)*cur->l->b;
	    }
	}

      cur = cur->n;
    }

  r_reflect.start->x = p->x + EPSILON * r_reflect.dir->x;
  r_reflect.start->y = p->y + EPSILON * r_reflect.dir->y;
  r_reflect.start->z = p->z + EPSILON * r_reflect.dir->z;
  traceRay(&r_reflect,&color_tmp2,d-1); /* do reflection */
  r.dir = in; /* do refraction */
  r.start->x = p->x + EPSILON * r.dir->x;
  r.start->y = p->y + EPSILON * r.dir->y;
  r.start->z = p->z + EPSILON * r.dir->z;
  if(m->trans > 0) /* speeds up rendering if object is completely opaque */
    traceRay(&r,&color_tmp3,d-1);
  c->r += color_tmp.r + m->refl * color_tmp2.r + m->trans * color_tmp3.r;
  c->g += color_tmp.g + m->refl * color_tmp2.g + m->trans * color_tmp3.g;
  c->b += color_tmp.b + m->refl * color_tmp2.b + m->trans * color_tmp3.b;

  /* clamp color values to 1.0 */
  if (c->r > 1.0) c->r = 1.0;
  if (c->g > 1.0) c->g = 1.0;
  if (c->b > 1.0) c->b = 1.0;
  if (c->r < 0.0) c->r = 0.0;
  if (c->g < 0.0) c->g = 0.0;
  if (c->b < 0.0) c->b = 0.0;
}

