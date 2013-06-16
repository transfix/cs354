#ifndef _DRAWING_H_
#define _DRAWING_H_

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#define NUMSLICES 3
#define DEGINC 2*M_PI/NUMSLICES
#define ALLOC_POINT(p) if((p=(struct point_t *)calloc(1,sizeof(struct point_t)))==NULL) \
    { \
      fprintf(stderr,"Error allocating memory.\n"); \
      exit(-1); \
    }
#define ALLOC_SLICE(s) if((s=(struct slice_t *)calloc(1,sizeof(struct slice_t)))==NULL) \
	{ \
	  fprintf(stderr,"Error allocating memory.\n"); \
	  exit(-1); \
	}

struct point_t
{
  GLfloat x,y,z;
  GLfloat nx,ny,nz; /* average normal to the vertex */
  struct point_t *n;
};

struct slice_t
{
  struct point_t *line;
  struct slice_t *n;
};

/* Functions implemented in drawing.c */
void drawSurface(void);
void addPoint(GLfloat x, GLfloat y, GLfloat z);
void addSlice(struct point_t *slice);
void removePoint(void);
struct point_t *calc_point(struct point_t *p0, struct point_t *p1,
			   struct point_t *p2, struct point_t *p3);
void interleave(struct point_t *dest, struct point_t *src);
void freeSlice(struct point_t *slice);
void freeModel(void);
struct point_t *duplicate_slice(struct point_t *slice);
void cross_product(GLfloat *v1,GLfloat *v2,GLfloat *v3);
void normalize(GLfloat *v);
void sub_vec(GLfloat *v1,GLfloat *v2, GLfloat *v3);
void add_vec(GLfloat *v1,GLfloat *v2, GLfloat *v3);
void recompute_normals(void);

#endif	/* _DRAWING_H_ */
