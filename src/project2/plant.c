/*
 * plant.c
 * ---------
 * Contains function callbacks and bookkeeping used by glut.
 *
 * Group Members: Jose Rivera
 */

#include "drawplant.h"
#include "matrix.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <signal.h>
#if 0
#include <sys/time.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#define MOUSE_ROTATE_Y 0
#define FOV 90
#define W 640
#define H 480

/* The current mode the mouse is in, based on what button(s) is pressed */
int mouse_mode;

/* The last position of the mouse since the last callback */
int m_last_x, m_last_y;


#define SPEED 0.5
#define ANG_SPEED .5

/* model origin */
float modelx;
float modely;
float modelz;

/* camera direction */
float camerax;
float cameray;
float cameraz;
float theta; /* angle in the yz plane */
float phi;   /* angle in the xz plane */

int p_mode = 0; /* projection mode (0 - perspective, 1 - orthographic) */

extern int lsys_depth;
extern int do_growth;
extern double growth;

/* local function declarations */
void display(void);
void init(void);
void resetCamera(void);
void keyHandler(unsigned char c,int x, int y);
void specialKeyHandler(int c, int x, int y);
void mouseHandler(int button, int state, int x, int y);
void motionHandler(int x, int y);

int main (int argc, char** argv) {
  glutInit(&argc,argv);
  glutInitWindowSize(W, H);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("plant");
  init();
  glutDisplayFunc(display);
  glutKeyboardFunc(keyHandler);
  glutSpecialFunc(specialKeyHandler);
  glutMouseFunc(mouseHandler);
  glutMotionFunc(motionHandler);
  glutMainLoop();
  return 0;
}

void init()
{
#if 0
  /* the timer for animation */
  struct itimerval timer;
  struct sigaction sv;
  /* set up the timer so we can update the animation at specified intervals
     without much trouble */
  memset(&sv, 0, sizeof(struct sigaction));
  sv.sa_flags = 0;
  sigemptyset(&sv.sa_mask);
  sv.sa_handler = time_quantum;
  sigaction(SIGALRM, &sv, NULL);
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = INTERVAL;
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = INTERVAL;
  setitimer(ITIMER_REAL,&timer,NULL);
#endif

  glutTimerFunc(INTERVAL/1000,time_quantum,0);

  glClearColor(0.0, 0.0, 0.0, 0.0);  
  resetCamera();
  lsys_depth = 1;
  do_growth = 0;
  growth = 1;
}

void resetCamera()
{
  modelx = 0.0;
  modely = -45.0;
  modelz = -50.0;
  camerax = 0.0;
  cameray = 0.0;
  cameraz = -1.0;
  theta = 90;
  phi = 180;
  p_mode=0;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(FOV,W/H,1,4096);
  printf("Perspective mode selected.\n");
  glMatrixMode(GL_MODELVIEW);
  LoadIdentity();
  Translate(modelx,modely,modelz);
  LoadMatrix();
}

void display()
{
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glFrontFace(GL_CW);
  glEnable(GL_CULL_FACE); /* hopefully speed up the rendering */
  glMatrixMode(GL_MODELVIEW);

  drawPlant(lsys_depth,1,1,0); 
  
  glFlush();  /* Flush all executed OpenGL ops finish */
  
  /*
   * Since we are using double buffers, we need to call the swap
   * function every time we are done drawing.
   */
  glutSwapBuffers();
}

void keyHandler(unsigned char c, int x, int y)
{
  /* translate the modelview, not the projection */
  /* When 'moving' the camera towards the model, translate the
     model in the opposite direction of the camera position vector.
     When strafing the camera left and right, translate the model
     in the direction of the cross product of the camera direction
     vector and the model position vector */
  /* Note: this doesn't quite work... moving up, down and forward/back
     seems to work, but strafing and turning the camera messes things
     up. */
  float v1[] = { camerax, cameray, cameraz };
  float v2[] = { modelx, modely, modelz };
  float v3[3];
  CrossProduct(v2,v1,v3);
  Normalize(v3);

  glMatrixMode(GL_MODELVIEW);

  switch(c)
    {
    case 'r':
      glMatrixMode(GL_MODELVIEW);
      modelx-=camerax*SPEED; modely-=cameray*SPEED; modelz-=cameraz*SPEED;
      Translate(-camerax*SPEED,-cameray*SPEED,-cameraz*SPEED);
      break;
    case 'f':
      glMatrixMode(GL_MODELVIEW);
      modelx+=camerax*SPEED; modely+=cameray*SPEED; modelz+=cameraz*SPEED;
      Translate(camerax*SPEED,cameray*SPEED,cameraz*SPEED);
      break;
    case 'd':
      glMatrixMode(GL_MODELVIEW);
      modelx+=v3[0]; modely+=v3[1]; modelz+=v3[2];
      Translate(v3[0]*SPEED,v3[1]*SPEED,v3[2]*SPEED);
      break;
    case 'g':
      glMatrixMode(GL_MODELVIEW);
      modelx-=v3[0]; modely-=v3[1]; modelz-=v3[2];
      Translate(-v3[0]*SPEED,-v3[1]*SPEED,-v3[2]*SPEED);
      break;
    case 'p': /* change projection mode */
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      switch(p_mode=(~p_mode)&1)
	{
	case 0:
	  gluPerspective(FOV,W/H,1,4096);
	  printf("Perspective mode selected.\n");
	  break;
	case 1:
	  glOrtho(-50,50,-50,50,1,4096);
	  printf("Orthographic mode selected.\n");
	  break;
	}
      glMatrixMode(GL_MODELVIEW);
      break;
    case 'h':
      if(!do_growth)
	{
	  do_growth=1;
	  printf("Growth started.  Press 'h' again to stop growth.\n");
	  growth=1;
	  lsys_depth=LSYS_DEFAULT;
	}
      else
	{ 
	  do_growth=0;
	  growth=1;
	  printf("Growth stopped. Press 'h' again to restart growth.\n");
	}
      break;
    case 'a':
      if(do_growth) 
	{
	  printf("Press 'h' to disable growth first.\n");
	  break; /* don't disturb the growth */
	}
      lsys_depth = (lsys_depth + 1) % (LSYS_DEPTH+1);
      if(lsys_depth==0) lsys_depth=LSYS_DEFAULT;
      printf("L-system depth changed to %d.\n",lsys_depth);
      break;
    case 's':
      if(do_growth)
	{
	  printf("Press 'h' to stop growth first.\n");
	  break; /* dont disturb the growth */
	}      
      lsys_depth--;
      if(lsys_depth<LSYS_DEFAULT) 
	lsys_depth=LSYS_DEPTH;
      printf("L-system depth changed to %d.\n",lsys_depth);
      break;
    case 'c':
      resetCamera();
      break;
    case '/':
      Rotate(ANG_SPEED,0.0,1.0,0.0);
      break;
    case '?':
      Rotate(-ANG_SPEED,0.0,1.0,0.0);
      break;
    case 'q':
      exit(0);
    }
  
  LoadMatrix();
  glutPostRedisplay();
}

/* non ascii keys */
void specialKeyHandler(int c, int x, int y)
{
  switch(c)
    {
    case GLUT_KEY_LEFT:
      theta+=ANG_SPEED;
      glMatrixMode(GL_PROJECTION);
      glRotatef(-ANG_SPEED,0.0,1.0,0.0);
      break;
    case GLUT_KEY_RIGHT:
      theta-=ANG_SPEED;
      glMatrixMode(GL_PROJECTION);
      glRotatef(ANG_SPEED,0.0,1.0,0.0);
      break;
    case GLUT_KEY_UP:
      phi-=ANG_SPEED;
      glMatrixMode(GL_PROJECTION);
      glRotatef(-ANG_SPEED,1.0,0.0,0.0);
      break;
    case GLUT_KEY_DOWN:
      phi+=ANG_SPEED;
      glMatrixMode(GL_PROJECTION);
      glRotatef(ANG_SPEED,1.0,0.0,0.0);
      break;
    }

  camerax=(float)cos(theta*(M_PI/180))*sin(phi*(M_PI/180));
  cameray=(float)sin(theta*(M_PI/180))*sin(phi*(M_PI/180));
  cameraz=(float)cos(phi*(M_PI/180));

  glutPostRedisplay();
}

void mouseHandler(int button, int state, int x, int y)
{
  if(state == GLUT_DOWN)
    {
      m_last_x = x;
      m_last_y = y;
		
      if (button == GLUT_LEFT_BUTTON) 
	mouse_mode = MOUSE_ROTATE_Y;
    }
}

void motionHandler(int x, int y)
{
  double d_x, d_y;
  
  d_x = x - m_last_x;
  d_y = y - m_last_y;
  
  m_last_x = x;
  m_last_y = y;
  
  glMatrixMode(GL_MODELVIEW);

  if (mouse_mode == MOUSE_ROTATE_Y)
    {
      /* scaling factors */
      d_x /= 15.0;
      Rotate(d_x, 0.0, 1.0, 0.0);  /* y-axis rotation */
    }

  LoadMatrix();
  glutPostRedisplay();
}

/* end of plant.c */
