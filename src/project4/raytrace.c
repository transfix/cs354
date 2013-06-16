/******************************************************************/
/*         Main raytracer file                                    */
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
#include "raytrace.h"
#include "readppm.h"
#include "texture.h"

/* local functions */
void initScene(void);
void initCamera (int, int);
void display(void);
void init(int, int);
void drawScene(void);
void addObject(object *o,material *m,int bounded,GLfloat *bounds);
void addLight(light *l);

/* local data */

/* the scene */
light *ambient=NULL; /* the scene's ambient light */
obj_list *world=NULL; /* the list of objects making up a scene */
light_list *lights=NULL; /* the lights in the scene */
GLfloat bg_r,bg_g,bg_b; /* background color */
int depth=0; /* ray tracing depth */
int done=FALSE;

/* the viewing parameters: */
point* viewpoint;
GLfloat pnear;  /* distance from viewpoint to image plane */
GLfloat fovx;  /* x-angle of view frustum */
int width = 1280;     /* width of window in pixels */
int height = 1024;    /* height of window in pixels */

int main (int argc, char** argv) {
  int win;
  glutInit(&argc,argv);
  glutInitWindowSize(width,height);
  glutInitWindowPosition(100,100);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  win = glutCreateWindow("raytrace");
  glutSetWindow(win);
  init(width,height);
  glutDisplayFunc(display);
  glutMainLoop();
  return 0;
}

void init(int w, int h) {

  /* OpenGL setup */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
  glClearColor(0.0, 0.0, 0.0, 0.0);  

  /* low-level graphics setup */
  initCanvas(w,h);

  /* raytracer setup */
  initCamera(w,h);
  initScene();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);
  if(!done)
    drawScene();  /* draws the picture in the canvas */
  flushCanvas();  /* draw the canvas to the OpenGL window */
  glFlush();
}

void addObject(object *o,material *m,int bounded,GLfloat *bounds)
{
  obj_list *cur;
  if(world==NULL)
    {
      ALLOC_OBJ_NODE(world);
      world->obj = o;
      world->obj->m = m;
      return;
    }
  cur = world;
  while(cur->n!=NULL) cur = cur->n;
  ALLOC_OBJ_NODE(cur->n);
  cur = cur->n;
  cur->obj = o;
  cur->obj->m = m;

  setBoundaries(o,bounded,bounds);
}

void addLight(light *l)
{
  light_list *cur;
  if(lights==NULL)
    {
      ALLOC_LIGHT_NODE(lights);
      lights->l = l;
      return;
    }
  cur = lights;
  while(cur->n!=NULL) cur = cur->n;
  ALLOC_LIGHT_NODE(cur->n);
  cur = cur->n;
  cur->l = l;
}

void initScene () {
  GLubyte *tex;
  int t_w, t_h;
  GLfloat cyl_bound[] = { 0.0,0.0,-2.0,0.5,0.0,0.0 }; /* limit the cylinder's height */
  GLfloat coeff[] = { 1/(0.4*0.4), -1.0, 1/(0.4*0.4), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; /* cone */
  GLfloat cone_bound[] = { 0.0,0.0,-2.0,0.0,0.0,0.0 };

  /* background color */
  bg_r = bg_g = bg_b = 0;

  depth = 8;

  /* init objects */
  addObject(makeSphere(-0.0,-2.0,-12.5,0.5),makeMaterial(1.0,0.5,0.15,0.3,0.7,0.7,100,0.4,0.0,1.0),UNBOUNDED,NULL);
  /* bottom plane */
  tex = readPPMfile(TEXTURE_FILE,&t_w,&t_h);
  addObject(makeInfinitePlane(0.0,-2.0,0.0,0.0,1.0,0.0),
	    makeMaterial(1.0,0.3,0.13,0.3,0.4,0.7,300,0.2,0.0,1.0),UNBOUNDED,NULL);
  /* left plane */
  addObject(makeInfinitePlane(-3.0,0.0,-15.0,1.0,0.0,0.1),
	    //makeMaterial(1.0,0.3,0.13,0.3,0.7,0.9,100,0.2,0.0,1.0),UNBOUNDED,NULL);
	    makeTexturedMaterial(0.5,0.3,0.13,0.3,0.7,0.9,100,0.2,0.0,1.0,tex,t_w,t_h),UNBOUNDED,NULL);
  /* right plane */
  addObject(makeInfinitePlane(4.0,0.0,-15.0,-1.0,0.0,0.1),
	    //makeMaterial(1.0,0.3,0.13,0.3,0.7,0.9,100,0.2,0.0,1.0),UNBOUNDED,NULL);
	    makeTexturedMaterial(0.5,0.3,0.13,0.3,0.7,0.9,100,0.2,0.0,1.0,tex,t_w,t_h),UNBOUNDED,NULL);
  /* bounded cylinder */
  addObject(makeEllipsoid(1.0,0.0,-12.0,0.1,0.0,0.1),makeMaterial(0.0,0.1,0.7,0.3,0.6,0.3,30,0.3,0.0,1.0),
	    Y_BOUNDED,cyl_bound);
  addObject(makeEllipsoid(-0.5,0.0,-9.0,0.1,0.0,0.1),makeMaterial(0.0,0.1,0.7,0.3,0.6,0.3,30,0.3,0.0,1.0),
	    Y_BOUNDED,cyl_bound);
  addObject(makeEllipsoid(-0.8,0.0,-5.0,0.1,0.0,0.1),makeMaterial(0.0,0.1,0.7,0.3,0.6,0.3,30,0.3,0.0,1.0),
	    Y_BOUNDED,cyl_bound);
  addObject(makeSphere(-1.0,-0.6,-21.0,5),makeMaterial(0.2,0.2,0.2,0.3,0.7,0.6,500,0.2,0.9,1.0),UNBOUNDED,NULL);
  //addObject(makeQuadratic(0.0,0.0,-40,coeff),makeMaterial(0.4,0.4,0.4,0.3,0.9,0.2,100,0.4,0.0,0.0),UNBOUNDED,NULL);
  addObject(makeQuadratic(3.0,0.0,-15.0,coeff),makeMaterial(0.7,0.1,0.7,0.6,0.9,0.4,100,0.4,0.1,0.0),
  	    Y_BOUNDED,cone_bound);

  /* init ambient light */
  ambient = makeLight(0.1,0.1,0.1,NULL,NULL);

  /* init lights */
  addLight(makeLight(0.7,0.7,0.7,makePoint(2.0,0.0,-2.0),makePoint(-1.0,0.0,0.0)));
  addLight(makeLight(0.7,1.0,0.3,makePoint(0.5,2.0,0.0),makePoint(-0.2,-1.0,0.0)));
  //addLight(makeLight(0.5,0.3,0.5,makePoint(6.0,1.0,-25.0),makePoint(-0.3,-0.1,1.0)));
}

void initCamera (int w, int h) {
  viewpoint = makePoint(0.0,0.0,0.0);
  pnear = 1.0;
  fovx = PI/4;
}

void drawScene () {
  int i,j;
  GLfloat imageWidth;
  /* declare data structures on stack to avoid dynamic allocation */
  point worldPix;  /* current pixel in world coordinates */
  point direction; 
  ray r; /* main ray */
  color c; /* main color */
#ifdef ANTI_ALIASING
  int k;
  ray sub_r[AA_RAYS]; /* rays for anti aliasing */
  color sub_c[AA_RAYS]; /* color for each ray */
  color avg_c; /* average color */
#endif

  /* initialize */
  worldPix.w = 1.0;
  worldPix.z = -pnear;

  r.start = &worldPix;
  r.dir= &direction;

  imageWidth = 2*pnear*tan(fovx/2);

  /* trace a ray for every pixel */
  for (i=0; i<width; i++) {
    /* Refresh the display */
    /* Comment this line out after debugging */
    flushCanvas();

    for (j=0; j<height; j++) {

      c.r = c.g = c.b = 0.0;
#ifdef ANTI_ALIASING
      avg_c.r = avg_c.g = avg_c.b = 0.0;
#endif

      /* find position of pixel in world coordinates */
      /* y position = (pixel height/middle) scaled to world coords */ 
      worldPix.y = (j-(height/2))*imageWidth/width;
      /* x position = (pixel width/middle) scaled to world coords */ 
      worldPix.x = (i-(width/2))*imageWidth/width;

      /* find direction */
      calculateDirection(viewpoint,&worldPix,&direction);

      /* trace the ray! */
      traceRay(&r,&c,depth);

#ifdef ANTI_ALIASING
      /* trace the anti-aliasing rays! */
      for(k=0; k<AA_RAYS; k++)
      	{
      	  sub_r[k].start = &worldPix;
	  sub_r[k].dir = (vector *)malloc(sizeof(vector));
      	  sub_r[k].dir->x = direction.x+AA_RAY_OFFSET*cos(k*DEG_INC);
	  sub_r[k].dir->y = direction.y+AA_RAY_OFFSET*sin(k*DEG_INC);
	  sub_r[k].dir->z = direction.z;
	  sub_r[k].dir->w = 0.0;
	  sub_c[k].r = sub_c[k].g = sub_c[k].b = 0.0;
	  traceRay(&sub_r[k],&sub_c[k],depth);
	  avg_c.r += sub_c[k].r;
	  avg_c.g += sub_c[k].g;
	  avg_c.b += sub_c[k].b;
	  free(sub_r[k].dir);
	}

      /* average the color values */
      avg_c.r += c.r; avg_c.g += c.g; avg_c.b += c.b;
      avg_c.r /= AA_RAYS+1; avg_c.g /= AA_RAYS+1; avg_c.b /= AA_RAYS+1;

      /* write the pixel! */
      drawPixel(i,j,avg_c.r,avg_c.g,avg_c.b);
#else
      /* write the pixel! */
      drawPixel(i,j,c.r,c.g,c.b);
#endif
    }
  }

  done=TRUE;
}

/* returns the color seen by ray r in parameter c */
/* d is the recursive depth */
void traceRay(ray* r, color* c, int d) {
  point p;  /* first intersection point */
  vector n;
  material* m;

  if(d==0) return;

  p.x = p.y = p.z = p.w = 0.0;  /* inialize to "no intersection" */
  boundaryHit(r,&p,&n,&m);
  //firstHit(r,&p,&n,NULL,&m);
  if (p.w != 0.0 && c != NULL) {
    shade(&p,&n,m,r->dir,c,d);  /* do the lighting calculations */
  } else {             /* nothing was hit */
    c->r = bg_r;
    c->g = bg_g;
    c->b = bg_b;
  }
}

/* boundaryHit */
/* keeps casting rays in the same direction until something is hit within 
   it's boundary */
void boundaryHit(ray* r, point* p, vector* n, material **m)
{
  ray tmp; /* we cannot change ray start point, so lets use a tmp ray */
  point start;
  vector dir;
  object *o=NULL;

  tmp.start = &start;
  tmp.dir = &dir;
  memcpy(tmp.start,r->start,sizeof(point));
  memcpy(tmp.dir,r->dir,sizeof(vector));
  do
    {
      firstHit(&tmp,p,n,&o,m);
      tmp.start->x = p->x + EPSILON * tmp.dir->x;
      tmp.start->y = p->y + EPSILON * tmp.dir->y;
      tmp.start->z = p->z + EPSILON * tmp.dir->z;
    }
  while(!checkBounds(o,p)); /* disregard hits unless they are within boundaries */
} 

/* firstHit */
/* If something is hit, returns the finite intersection point p, 
   the normal vector n to the surface at that point, and the surface
   material m. If no hit, returns an infinite point (p->w = 0.0) */
void firstHit(ray* r, point* p, vector* n, object **o, material* *m) {
  double t = DBL_MAX;  /* parameter value at first hit */
  double t_test;
  int hit = FALSE,retval;
  obj_list *cur = world;
  object *closest = NULL;
  ray tmp;
  point start;
  vector dir;

  tmp.start = &start;
  tmp.dir   = &dir;

  while(cur!=NULL)
    {
      switch(cur->obj->type)
	{
	case QUADRATIC:
	  retval = rayQuadraticIntersect(r,cur->obj,&t_test);
	  break;
	case PLANE:
	  retval = rayPlaneIntersect(r,cur->obj,&t_test);
	  break;
	default:
	  break;
	}
  
      if(retval)
	{
	  if(t_test < t) /* make sure we get the closest object to ray origin */
	    {
	      closest = cur->obj;
	      t = t_test;
	      hit = TRUE;
	    }
	}

      cur = cur->n;
    }
  
  if(hit)
    {
      if(n==NULL || m==NULL) /* if n or m is null, just return hit status */
	  findPointOnRay(r,t,p);
      else
	{
	  *m = closest->m;
	  findPointOnRay(r,t,p);
	  switch(closest->type)
	    {
	    case QUADRATIC:
	      findQuadraticNormal(r,closest,p,n);
	      break;
	    case PLANE:
	      findPlaneNormal(r,closest,p,n);
	    default:
	      break;
	    }
	}
      if(o!=NULL)
	*o = closest;
    }
  else/* nothing hit */
    p->w = 0.0;
}

