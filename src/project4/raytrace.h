#ifndef _RAYTRACE_H_
#define _RAYTRACE_H_

#define ALLOC_OBJ_NODE(x) if((x=(struct obj_list *)calloc(1,sizeof(struct obj_list)))==NULL) \
    { \
      fprintf(stderr,"Error allocating memory.\n"); \
      exit(-1); \
    }

#define ALLOC_LIGHT_NODE(x) if((x=(struct light_list *)calloc(1,sizeof(struct light_list)))==NULL) \
    { \
      fprintf(stderr,"Error allocating memory.\n"); \
      exit(-1); \
    }

#define ALLOC_OBJ(x) if((x=(struct object *)calloc(1,sizeof(struct object)))==NULL) \
    { \
      fprintf(stderr,"Error allocating memory.\n"); \
      exit(-1); \
    }

/******************************************************************/
/*         Raytracer declarations                                 */
/******************************************************************/


/* constants */
#define TRUE 1
#define FALSE 0

#define EPSILON 0.001
#define PI 3.14159265358979323846264338327
#ifndef DBL_MAX
#define DBL_MAX         1.7976931348623157E+308
#endif
#define AA_RAYS 15
#define AA_RAY_OFFSET 0.0005
#define DEG_INC (2*PI)/AA_RAYS
#define ANTI_ALIASING
#define TEX_SCALE 100.0

/* data structures */

typedef struct point {
  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLfloat w;
} point;

/* a vector is just a point */
typedef point vector;

/* a ray is a start point and a direction */
typedef struct ray {
  point* start;
  vector* dir;
} ray;

typedef struct material { /* for all x 0<=x<=1 */
  /* color */
  GLfloat r;
  GLfloat g;
  GLfloat b; 
  GLfloat amb;   /* ambient reflectivity */
  GLfloat diff;  /* diffuse coefficient */
  GLfloat spec;  /* specular coefficient */
  GLfloat ns;    /* shininess */
  GLfloat refl;  /* reflectivity coefficient */
  GLfloat trans; /* transparency */
  GLfloat n;     /* index of refraction */

  int textured; /* if set to true, use 'texture' for the texture map */
  GLubyte *texture; /* texture map for this object */
  int t_w; /* texture width */
  int t_h; /* texture height */
} material;

typedef struct color {
  GLfloat r;
  GLfloat g;
  GLfloat b; 
  /* these should be between 0 and 1 */
} color;

/* object type definitions */
#define POLYGON 0
#define QUADRATIC 1
#define PLANE 2

/* bounded defs */
#define UNBOUNDED 0
#define X_BOUNDED 1
#define Y_BOUNDED 2
#define Z_BOUNDED 4

/* general object type - spare efficiency for elegance */
typedef struct object {
  int type; /* object type */

  /* for polygons */
  vector *n; /* plane normal */
  point *v; /* verticies */

  /* for quadratic surfaces and planes - f(x,y,z) = 
     A(x-x0)^2+B(y-y0)^2+C(z-z0)^2+D(x-x0)(y-y0)+E(x-x0)(z-z0)+F(y-y0)(z-z0)+G(x-x0)+H(y-y0)+I(z-z0)+J=0 */
  point *c; /* center (x0,y0,z0) */
  GLfloat k[10]; /* constants A thru J (for planes, variables A-D are used) */

  material *m;

  int bounded; /* low bit set == x bounded, second bit set == y bounded, third bit set == z bounded */
  GLfloat bounds[6]; /* first 2 == x0,x1 ; second 2 == y0,y1 ; third 2 == z0,z1 */

  long id; /* for debugging purposes */
} object;

/* quadratic constant defs */
#define A(s) s->k[0]
#define B(s) s->k[1]
#define C(s) s->k[2]
#define D(s) s->k[3]
#define E(s) s->k[4]
#define F(s) s->k[5]
#define G(s) s->k[6]
#define H(s) s->k[7]
#define I(s) s->k[8]
#define J(s) s->k[9]

/* a list of objects */
typedef struct obj_list {
  object *obj; /* this object */
  struct obj_list *n; /* next object */
} obj_list;

typedef struct light {
  point *p; /* location of light */
  vector *d; /* direction of light */
  GLfloat r,g,b; /* light intensity */
} light;

typedef struct light_list {
  light *l; /* current light */
  struct light_list *n; /* next light */
} light_list;

/* functions in raytrace.c */
void traceRay(ray*, color*, int);
void firstHit(ray*,point*,vector*, object **o,material**);
void boundaryHit(ray* r, point* p, vector* n, material **m);

/* functions in geometry.c */
object *makeQuadratic(GLfloat x, GLfloat y, GLfloat z,GLfloat *coeff);
object* makeSphere(GLfloat, GLfloat, GLfloat, GLfloat);
object *makeEllipsoid(GLfloat x, GLfloat y, GLfloat z, GLfloat a, GLfloat b, GLfloat c);
object *makeInfinitePlane(GLfloat x, GLfloat y, GLfloat z, GLfloat a, GLfloat b, GLfloat c);
point* makePoint(GLfloat, GLfloat, GLfloat);
point* copyPoint(point *);
void freePoint(point *);
void calculateDirection(point*,point*,point*);
void normalize(vector *v);
float dotProduct(point *p, point *q);
void findPointOnRay(ray*,double,point*);
int rayQuadraticIntersect(ray*,object*,double*);
void findQuadraticNormal(ray*,object*,point*,vector*);
int rayPlaneIntersect(ray* r,object* s,double* t);
void findPlaneNormal(ray*,object*,point*,vector*);
void setBoundaries(object *o, int bounded, GLfloat *bounds);
int checkBounds(object *o, point *p);

/* functions in light.c */
material *makeMaterial(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat diff, 
		       GLfloat spec, GLfloat ns, GLfloat refl,GLfloat trans,GLfloat n);
material *makeTexturedMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat amb, 
			       GLfloat diff, GLfloat spec, GLfloat ns, GLfloat refl,
			       GLfloat trans,GLfloat n,GLubyte *texture,int t_w,int t_h);
light *makeLight(GLfloat r, GLfloat g, GLfloat b, point *p, vector *d);
void shade(point*,vector*,material*,vector*,color*,int);

/* global variables */
extern int width;
extern int height;

#endif	/* _RAYTRACE_H_ */
