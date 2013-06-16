/*
 * mouse.c
 * -------
 * Mouse callbacks for the canvas.  Currently a little clunky, because
 * all rotations are in terms of the original axes of the model, rather
 * than the transformed axes of the current viewpoint.
 *
 * You shouldn't have to modify anything in this file.
 */

#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include "mouse.h"

/* The frustum and zoom factor variables from canvas.c */
extern GLfloat fleft;
extern GLfloat fright;
extern GLfloat ftop;
extern GLfloat fbottom;
extern GLfloat zNear;
extern GLfloat zFar;
extern GLfloat zoomFactor;

void myMouseButton(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		m_last_x = x;
		m_last_y = y;

		if (button == GLUT_LEFT_BUTTON) {
			mouse_mode = MOUSE_ROTATE_YX;
		} else if (button == GLUT_MIDDLE_BUTTON) {
			mouse_mode = MOUSE_ROTATE_YZ;
		} else if (button == GLUT_RIGHT_BUTTON) {
			mouse_mode = MOUSE_ZOOM;
		}
	}
}

void myMouseMotion(int x, int y) {
	double d_x, d_y;	/* The change in x and y since the last callback */

	d_x = x - m_last_x;
	d_y = y - m_last_y;

	m_last_x = x;
	m_last_y = y;

	if (mouse_mode == MOUSE_ROTATE_YX) {
		/* scaling factors */
		d_x /= 2.0;
		d_y /= 2.0;

		glRotatef(d_x, 0.0, 1.0, 0.0);	/* y-axis rotation */
		glRotatef(-d_y, 1.0, 0.0, 0.0);	/* x-axis rotation */

	} else if (mouse_mode == MOUSE_ROTATE_YZ) {
		/* scaling factors */
		d_x /= 2.0;
		d_y /= 2.0;

		glRotatef(d_x, 0.0, 1.0, 0.0);	/* y-axis rotation */
		glRotatef(-d_y, 0.0, 0.0, 1.0);	/* z-axis rotation */

	} else if (mouse_mode == MOUSE_ZOOM) {
		d_y /= 100.0;

		zoomFactor += d_y;

		if (zoomFactor <= 0.0) {
			/* The zoom factor should be positive */
			zoomFactor = 0.001;
		}

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		/*
		 * glFrustum must receive positive values for the near and far
		 * clip planes ( arguments 5 and 6 ).
		 */
		glFrustum(fleft*zoomFactor, fright*zoomFactor,
			fbottom*zoomFactor, ftop*zoomFactor,
			-zNear, -zFar);
	}

	/* Redraw the screen */
	glutPostRedisplay();
}

/* end of mouse.c */

