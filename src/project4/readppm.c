/*
 * readppm.c
 * -----------
 * This function takes the name of a file as input. It should be
 * file in RAW .ppm format - a binary RGB file type (you can convert
 * many kinds of image files to raw .ppm using "xv" or "convert" command 
 * on the CS department machines).
 *
 * It opens the file and reads the contents into an array of unsigned
 * bytes, three for each pixel. It returns a pointer to the array.
 *
 * The array can be displayed in OpenGL window using the 
 * glDrawPixels command.
 *
 * Using this routine is not mandatory.  It is intended to help you in case
 * you wish to use .ppm files as an Additional Feature.
 *
 * Group Members: <FILL IN>
 */

#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>

/*
 * Loads the PPM file specified in "filename" into an array
 * of pixels compatible with glDrawPixels().  The two arguments
 * "wp" and "hp" should point to two integers that have already
 * been allocated in the caller of this function.  The ints they
 * point to are set to the image width and image height, respectively.
 *
 * Usage:
 *
 *     int pic_w, pic_h;
 *     GLubyte* pixels;
 *     pixels = readPPMfile("myFile.ppm", &pic_w, &pic_h);
 */
GLubyte* readPPMfile(char* filename, int *wp, int *hp) {
	FILE* input;
	int w,h,max,i,j,k;
	char rgb[3];
	GLubyte* pixels;
	char buffer[200];

	/* tell OpenGL we're using 3 bytes to a pixel, no padding */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if ((input = fopen(filename,"r")) == NULL) {
		fprintf(stderr,"Cannot open file %s \n",filename);
		exit(1);
	}

	/* read a line of input */
	fgets(buffer,200,input);
	if (strncmp(buffer,"P6",2) != 0) {
		fprintf(stderr,"%s is not a binary PPM file \n",filename);
		exit(1);
	}

	/* get second line, ignoring comments */
	do {
		fgets(buffer,200,input);
	} while (strncmp(buffer,"#",1) == 0);

	if (sscanf(buffer,"%d %d",&w,&h) != 2) {
		fprintf(stderr,"can't read sizes! \n");
		exit(1);
	}

	/* third line, ignoring comments */
	do {
		fgets(buffer,200,input);
	} while (strncmp(buffer,"#",1) == 0);

	if (sscanf(buffer,"%d",&max) != 1) {
		fprintf(stderr,"what about max size? \n");
		exit(1);
	}

	fprintf(stderr,"reading %d columns %d rows \n",w,h);

	pixels = (GLubyte*) malloc(w*h*3*sizeof(GLubyte));
	for (i=0; i<h; i++) {
		for (j=0; j<w; j++) {
			fread(rgb,sizeof(char),3,input);
			for (k=0; k<3; k++)	{
				*(pixels+(h-1-i)*w*3+j*3+k) = (GLubyte) rgb[k];
			}
		}
	}

	*wp = w;
	*hp = h;
	return(pixels);
}

/* end of readppm.c */
