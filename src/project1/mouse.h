#ifndef _MOUSE_H_
#define _MOUSE_H_

/* You shouldn't have to modify anything in this file. */

/* All variables / contants declared in this file are global. */

/* The three different operations the mouse can control for the canvas */
#define MOUSE_ROTATE_YX		0
#define MOUSE_ROTATE_YZ		1
#define MOUSE_ZOOM			2

/* The current mode the mouse is in, based on what button(s) is pressed */
int mouse_mode;

/* The last position of the mouse since the last callback */
int m_last_x, m_last_y;

/* Function declarations */
void myMouseButton(int button, int state, int x, int y);
void myMouseMotion(int x, int y);

#endif	/* _MOUSE_H_ */

