/*
 * subdiv.c
 * ----------
 * Contains function callbacks and bookkeeping for window management.
 *
 * Group Members: <FILL IN>
 */

#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include "drawing.h"

#define MOUSE_ROTATE_YX 0
#define MOUSE_ROTATE_YZ	1
#define MOUSE_ZOOM	2		
#define FOV 80

/* GLOBAL VARAIBLES */
int W=400;		/* window width */
int H=400;		/* window height */
int X_OFF = 10;	/* window x offset */
int Y_OFF = 10;	/* window y offset */

int mode = 0; /* current mode: 0 - 2D mode, 1 - 3D mode */
int solid = 1; /* if == 0, draw in wireframe mode */
int faces = 1; /* if == 0, draw control points only */
int normals = 0; /* if == 1, draw normal vectors */
int shading = 0; /* Gouraud - 0, Phong - 1 */

/* Global zoom factor.  Modified by user input. Initially 1.0 */
GLfloat zoomFactor = 1.0; 

 /* The current mode the mouse is in, based on what button(s) is pressed */
int mouse_mode;

/* The last position of the mouse since the last callback */
int m_last_x, m_last_y;

int num = 0; /* number of control points */

/* The dimensions of the viewing frustum */
GLfloat fleft   = -40.0;
GLfloat fright  =  40.0;
GLfloat fbottom = -40.0;
GLfloat ftop    =  40.0;
GLfloat zNear   =  -1;
GLfloat zFar    =  -4096;

GLfloat dpz; /* distance from camera to drawing plane (drawing plane is at z=0) */
GLfloat dptop; /* drawing plane top */
GLfloat dpbottom; /* drawing plane bottom */
GLfloat dpleft; /* drawing plane left */
GLfloat dpright; /* drawing plane right */

extern struct point_t *line; /* the line drawn on the plane */
extern struct slice_t *slices; /* a list of slices that make up the shape */
extern int subdiv_v; /* The user-specified subdivision level, vertical */
extern int subdiv_h; /* The user-specified subdivision level, horizontal */

/* local function declarations */
void init(void);
void resetCamera(void);
void display(void);
void myKeyHandler(unsigned char ch, int x, int y);
void myMouseButton(int button, int state, int x, int y);
void motionHandler(int x, int y);
void endSubdiv(int status);
void reshape(int width, int height);

int main (int argc, char** argv)
{
  glutInit(&argc,argv);
  glutInitWindowSize(W, H);
  //glutInitWindowPosition(X_OFF, Y_OFF);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("subdivisions");
  init();
  resetCamera();
  glutDisplayFunc(display);
  glutKeyboardFunc(myKeyHandler);
  glutMouseFunc(myMouseButton);
  glutMotionFunc(motionHandler);
  glutReshapeFunc(reshape);
  glutMainLoop();
  return 0;
}

void init()
{
  glShadeModel (GL_SMOOTH); // aka Gouraud shading

  dpz = -50;
  dptop = ftop;
  dpbottom = fbottom;
  dpleft = fleft;
  dpright = fright;
}

void resetCamera()
{
  glClearColor(0.0, 0.0, 0.0, 0.0);  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(fleft, fright, fbottom, ftop, -zNear, -zFar);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0,0.0,dpz);
  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT0);
}

void reset3DCamera()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(FOV,(double)(W/H),-zNear,-zFar);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0,0.0,dpz);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
}

void reshape(int width, int height)
{
  W=width;
  H=height;
  glViewport(0,0,W,H);
  glutPostRedisplay();
}

void display()
{
  // Set up material (surface) property
  GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat mat_shininess[] = { 50.0 };
  // Set up light
  GLfloat light_ambient[] = { 0.0, 1.0, 0.0, 1.0 };
  GLfloat light_diffuse[] = { 0.1, 1.0, 0.1, 1.0 };
  GLfloat light_specular[] = { 0.4, 0.5, 0.8, 1.0 };
  GLfloat light_position[] = { 50.0, 50.0, 50.0, 0.0 };

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);
  //if(!solid) /* dont cull face in wireframe mode */
  //  glDisable(GL_CULL_FACE);
  //else
  //  glEnable(GL_CULL_FACE);
  //glCullFace(GL_BACK);
  glFrontFace(GL_CW); /* verticies are drawn clockwise */
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  
  glPushMatrix();
  glLoadIdentity();
  
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  
  glPopMatrix();

  drawSurface();

  glFlush();  /* Flush all executed OpenGL ops finish */

  /*
   * Since we are using double buffers, we need to call the swap
   * function every time we are done drawing.
   */
  glutSwapBuffers();
}

void myKeyHandler(unsigned char ch, int x, int y)
{
  int i;
  static int subdiv=0;
  struct point_t *slice;
  struct point_t *linecur;
  struct point_t *cur;
  struct point_t *new_points;

  struct slice_t *cur_slice,*cur2_slice;
  struct point_t *cur2;
  struct point_t points[5];
  double a,b,c;

  GLfloat v1[3],v2[3],v3[3];

  double deginc;

  //  struct slice_t *cur_slice;

  switch(ch)
    {
    case 'q':
      endSubdiv(0);
      break;

    case 'z':
      mode=(~mode)&1;
      printf("%s\n",mode?"3D mode":"2D mode");
      switch(mode)
	{
	case 0:
	  resetCamera();
	  break;
	case 1:
	  reset3DCamera();
	  break;
	}
      break;

    case 'k':
      /* test phong stuff */
      cur_slice = slices;
      cur2_slice = slices->n;
      //while(cur_slice!=NULL)
	{
	  cur = cur_slice->line;
	  cur2 = cur2_slice->line;
	  //while(cur->n!=NULL)
	    {
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

	      printf("v2[0]=%f,v2[1]=%f,v2[2]=%f\nv3[0]=%f,v3[1]=%f,v3[2]=%f\n",v2[0],v2[1],v2[2],v3[0],v3[1],v3[2]);

	      for(i=0; i<5; i++)
		printf("points[%d]->x=%f,points[%d]->y=%f,points[%d]->z=%f\n",
		       i,points[i].x,i,points[i].y,i,points[i].z);
	      printf("cur->x=%f,cur->y=%f,cur->z=%f\ncur->n->x=%f,cur->n->y=%f,cur->n->z=%f\n",
		     cur->x,cur->y,cur->z,cur->n->x,cur->n->y,cur->n->z);
	      printf("cur2->x=%f,cur2->y=%f,cur2->z=%f\ncur2->n->x=%f,cur2->n->y=%f,cur2->n->z=%f\n",
		     cur2->x,cur2->y,cur2->z,cur2->n->x,cur2->n->y,cur2->n->z);

	      cur = cur->n;
	      cur2 = cur2->n;
	    }

	  cur_slice = cur_slice->n;
	  cur2_slice = cur2_slice->n != NULL ? cur2_slice->n : slices; /* circle around */
	}
      break;

    case 'n':
      normals=(~normals)&1;
      printf("Normal mode %s\n",normals?"on":"off");
      break;

    case 'e':
      solid=(~solid)&1;
      printf("%s\n",solid?"Solid mode":"Wireframe mode");
      switch(solid)
	{
	case 0:
	  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	  break;
	case 1:
	  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	  break;
	}
      break;

    case 'r':
      faces=(~faces)&1;
      printf("%s\n",faces?"Faces mode":"Control points mode");
      break;

    case 'w': /* calculate initial 3d object */
      if(num<5)
	printf("There must be at least 5 control points.\n");
      else if(!mode)
	{
	  mode=(~mode)&1;
	  printf("%s\n",mode?"3D mode":"2D mode");
	  switch(mode)
	    {
	    case 0:
	      resetCamera();
	      break;
	    case 1:
	      reset3DCamera();
	      break;
	    }
	  freeModel();
	  subdiv_v = 0;
	  subdiv_h = NUMSLICES;

	  /* the radius of the circle for each of the points is x */
	  for(i=0;i<subdiv_h;i++)
	    {
	      ALLOC_POINT(slice);
	      cur=slice;

	      linecur=line;
	      while(linecur!=NULL)
		{
		  cur->z = linecur->x*sin(DEGINC*i);
		  cur->x = linecur->x*cos(DEGINC*i);
		  cur->y = linecur->y;

		  linecur = linecur->n;
		  if(linecur!=NULL)
		    {
		      ALLOC_POINT(cur->n);
		      cur = cur->n;
		    }
		}

	      addSlice(slice);
	    }
	}

      recompute_normals();

      break;
     
    case 's': /* horizontal subdivision */

      if(!mode || slices==NULL) break;

      /* backup the original slice */
      new_points = duplicate_slice(slices->line);
      freeModel();

      subdiv_h<<=1;
      subdiv++;
      printf("Horizontal subdivision level %d\n",subdiv);
     
      deginc = 2*M_PI/subdiv_h;

      for(i=0;i<subdiv_h;i++)
	{
	  ALLOC_POINT(slice);
	  cur=slice;

	  linecur=new_points;
	  while(linecur!=NULL)
	    {
	      cur->z = linecur->x*sin(deginc*i);
	      cur->x = linecur->x*cos(deginc*i);
	      cur->y = linecur->y;

	      linecur = linecur->n;
	      if(linecur!=NULL)
		{
		  ALLOC_POINT(cur->n);
		  cur = cur->n;
		}
	    }

	  addSlice(slice);
	}

      recompute_normals();

      break;

    case 'a': /* vertical subdivision */

      if(!mode || slices==NULL) break;
      cur_slice=slices;
      
      subdiv_v++;
      printf("Vertical subdivision level %d\n",subdiv_v);
            
      linecur = cur_slice->line;
      /* calc the first point */
      cur = new_points = calc_point(linecur,linecur,linecur->n,linecur->n->n);
      
      /* calc middle and last points */
      while(linecur->n->n!=NULL)
	{
	  if(linecur->n->n->n!=NULL) /* middle points */
	    cur->n = calc_point(linecur,linecur->n,linecur->n->n,linecur->n->n->n);
	  else
	    cur->n = calc_point(linecur,linecur->n,linecur->n->n,linecur->n->n);
	  cur = cur->n;
	  linecur = linecur->n;
	}
      
      interleave(cur_slice->line,new_points);
      
      new_points = duplicate_slice(cur_slice->line);
      
      deginc = 2*M_PI/subdiv_h;

      freeModel();

      for(i=0;i<subdiv_h;i++)
	{
	  ALLOC_POINT(slice);
	  cur=slice;

	  linecur=new_points;
	  while(linecur!=NULL)
	    {
	      cur->z = linecur->x*sin(deginc*i);
	      cur->x = linecur->x*cos(deginc*i);
	      cur->y = linecur->y;

	      linecur = linecur->n;
	      if(linecur!=NULL)
		{
		  ALLOC_POINT(cur->n);
		  cur = cur->n;
		}
	    }
	  
	  addSlice(slice);
	}

      recompute_normals();

	break;

    case 'd':
      shading=(~shading)&1;
      printf("%s shading\n",shading?"Phong":"Gouraud");
      break;

    case '<':
      if(mode)
	{
	  glMatrixMode(GL_MODELVIEW);
	  glRotatef(1,0.0,1.0,0.0);
	}
      break;
    case '>':
      if(mode)
	{
	  glMatrixMode(GL_MODELVIEW);
	  glRotatef(-1,0.0,1.0,0.0);
	}
      break;

    default:
      /* Unrecognized keypress */
      return;
    }
  
  glutPostRedisplay();
  
  return;
}

void myMouseButton(int button, int state, int x, int y)
{
  GLfloat px, py, pz;
  if (state == GLUT_DOWN)
    {
      if(!mode) /* 2D mode */
	{
	  if (button == GLUT_LEFT_BUTTON)
	    {
	      if((W-x-10)>=(W/2)) x = W/2;
	      printf("W-x-10 == %d\n",W-x-5);
	      px = dpleft + (x*fabs(dpleft-dpright))/W;
	      /* assuming that the axis of rotation is at px=0 */
	      if(px<0) px = 0;
	      py = dptop - (y*fabs(dptop-dpbottom))/H;
	      pz = 0;
	      addPoint(px,py,pz);
	      //printf("x: %3d, y: %3d, i: %3d\n",x,y,num++);
	      //printf("px: %3f, py: %3f, pz: %3f\n",px,py,pz);
	      num++;
	    }
	  else if(button==GLUT_RIGHT_BUTTON)
	    {
	      if(num>0)
		{
		  printf("removing control point %d\n",--num);
		  removePoint();
		}
	    }
	  glutPostRedisplay();
	}
      else /* 3D mode */
	{
	  m_last_x = x;
	  m_last_y = y;
	  
	  if (button == GLUT_LEFT_BUTTON) 
	    mouse_mode = MOUSE_ROTATE_YX;
	  else if (button == GLUT_MIDDLE_BUTTON)
	    mouse_mode = MOUSE_ROTATE_YZ;
	  else if (button == GLUT_RIGHT_BUTTON)
	    mouse_mode = MOUSE_ZOOM;
	}
    }
}

void motionHandler(int x, int y)
{
  double d_x, d_y;
 
  if(!mode) return;
 
  d_x = x - m_last_x;
  d_y = y - m_last_y;
  
  m_last_x = x;
  m_last_y = y;
  
  glMatrixMode(GL_MODELVIEW);

  if (mouse_mode == MOUSE_ROTATE_YX)
    {
      /* scaling factors */
      d_x /= 2.0;
      d_y /= 2.0;
      
      glRotatef(d_x, 0.0, 1.0, 0.0);	/* y-axis rotation */
      glRotatef(-d_y, 1.0, 0.0, 0.0);	/* x-axis rotation */
    }
  else if (mouse_mode == MOUSE_ROTATE_YZ) 
    {
      /* scaling factors */
      d_x /= 2.0;
      d_y /= 2.0;
      
      glRotatef(d_x, 0.0, 1.0, 0.0);	/* y-axis rotation */
      glRotatef(-d_y, 0.0, 0.0, 1.0);	/* z-axis rotation */
    }
  else if (mouse_mode == MOUSE_ZOOM) 
    {
      d_y /= 100.0;
      
      zoomFactor += d_y;
      if(FOV*zoomFactor >= 180) zoomFactor -= d_y;
      
      if (zoomFactor <= 0.0) 
	{
	  /* The zoom factor should be positive */
	  zoomFactor = 0.001;
	}
      
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      
      gluPerspective(FOV*zoomFactor,(double)(W/H),-zNear,-zFar);
    }

  glutPostRedisplay();
}

void endSubdiv(int status)
{
  printf("\nQuitting subdivision program.\n\n");
  fflush(stdout);
  
  exit(status);
}

/* end of plant.c */
