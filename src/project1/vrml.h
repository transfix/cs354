#ifndef _VRML_H_
#define _VRML_H_

/* All variables / contants declared in this file are global. */

/*
 * Constants for the different types of VRML objects.
 * The constants should begin at 0 and should be consecutive integers.
 * 
 * VR_MAX should be the total number of vrml objects.
 */
#define VR_CUBE			        0
#define VR_DODECAHEDRON		        1
#define VR_ICOSAHEDRON		        2
#define VR_PYRAMID			3
#define VR_MAX				4

/* The current vermal object */
int vr_object;


/* Function Declarations */
void draw_vrml_cube(void);
void draw_vrml_dodecahedron(void);
void draw_vrml_icosahedron(void);
void draw_vrml_pyramid(void);
void draw_vrml_gl(void);
void draw_vrml_object(GLfloat *vert,GLuint *ind,int faces,GLenum mode);
/* Print the current vrml object.  Used when the user selects a new object. */
void print_vrml_object(void);

#endif	/* _VRML_H_ */

