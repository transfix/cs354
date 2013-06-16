/*
 * matrix.c
 * -------------
 * Contains all of the matrix functions used for model
 * transformations.
 *
 * Group Members: Jose Rivera
 */

#include "matrix.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glut.h>

#include <string.h>
#include <stdlib.h>
#include <math.h>

/* row major order */
float identity[] = { 1, 0, 0, 0,
		     0, 1, 0, 0,
		     0, 0, 1, 0,
		     0, 0, 0, 1, };

/* the model view matrix */
float m[] =  { 1, 0, 0, 0,
	       0, 1, 0, 0,
	       0, 0, 1, 0,
	       0, 0, 0, 1, };

/* the matrix stack */
float *ms[STACK_SIZE];
float **msp=NULL; /* matrix stack pointer */

void RowToColumn(float *tmp)
{
  float _m[16];
  memcpy(_m,tmp,16*sizeof(float));
  _m[0] = tmp[0]; _m[1] = tmp[4]; _m[2] = tmp[8]; _m[3] = tmp[12];
  _m[4] = tmp[1]; _m[5] = tmp[5]; _m[6] = tmp[9]; _m[7] = tmp[13];
  _m[8] = tmp[2]; _m[9] = tmp[6]; _m[10] = tmp[10]; _m[11] = tmp[14];
  _m[12] = tmp[3]; _m[13] = tmp[7]; _m[14] = tmp[11]; _m[15] = tmp[15];
  memcpy(tmp,_m,16*sizeof(float));
}

void LoadMatrix()
{
  GLfloat _m[16];
  memcpy(_m,m,16*sizeof(float));
  RowToColumn(_m);
  glLoadMatrixf(_m);
}

void LoadIdentity()
{
  memcpy(m,identity,16*sizeof(float));
}

void MultMatrix(float *_m)
{
  GLfloat tmp[16];
  int i,j,k,newj=-4;
  memset(tmp,0,16*sizeof(float));
  for(i=0; i<16; i++)
    {
      if(!(i%4)) newj+=4;
      for(j=newj,k=i%4; j<newj+4; j++,k+=4)
	tmp[i] += m[j]*_m[k];
    }
  memcpy(m,tmp,16*sizeof(float));
}


/* rotate about a vector xi+yj+zk */
/* http://www.euclideanspace.com/maths/algebra/matrix/orthogonal/rotation/ */
void Rotate(float angle,float x,float y,float z)
{
  float _m[16];
  float cos1,xsin,ysin,zsin;
  cos1 = 1-cos(angle); xsin=x*sin(angle); ysin=y*sin(angle); zsin=z*sin(angle);
  memcpy(_m,identity,16*sizeof(float));
  _m[0] = 1+(cos1*(x*x-1));
  _m[1] = -zsin+cos1*x*y;
  _m[2] = ysin+cos1*x*z;
  _m[4] = zsin+cos1*x*y;
  _m[5] = 1+(cos1*(y*y-1));
  _m[6] = -xsin+cos1*y*z;
  _m[8] = -ysin+cos1*x*z;
  _m[9] = xsin+cos1*y*z;
  _m[10] = 1+(cos1*(z*z-1));
  MultMatrix(_m);
}

void Scale(float x, float y, float z)
{
  float _m[16];
  memcpy(_m,identity,16*sizeof(float));
  _m[0]=x; _m[5]=y; _m[10]=z;
  MultMatrix(_m);
}

void Translate(float x, float y, float z)
{
  float _m[16];
  memcpy(_m,identity,16*sizeof(float));
  _m[3]=x; _m[7]=y; _m[11]=z;
  MultMatrix(_m);
}

void PushMatrix()
{
  float *newm = (float *)malloc(16*sizeof(float));
  memcpy(newm,m,16*sizeof(float));
  msp=(msp==NULL?ms:msp); /* if msp is unset, start at top of matrix */
  *msp = newm;
  msp++;
}

void PopMatrix()
{
  if(msp==ms) return;
  msp--;
  memcpy(m,*msp,16*sizeof(float));
  free(*msp);
  *msp=NULL;
}

void CrossProduct(float *v1,float *v2,float *v3)
{
  v3[0] = v1[1]*v2[2]-v2[1]*v1[2];
  v3[1] = v1[0]*v2[2]-v2[0]*v1[2];
  v3[2] = v1[0]*v2[1]-v2[0]*v1[1];
}

void Normalize(float *v)
{
  float n = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
  v[0] = v[0]/n; v[1] = v[1]/n; v[2] = v[2]/n;
}
