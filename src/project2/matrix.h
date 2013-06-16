#ifndef MATRIX_H
#define MATRIX_H

#define STACK_SIZE 256

void RowToColumn(float *_m); /* converts _m to colum major order from row major order */
void LoadMatrix(); /* loads m into the GL modelview matrix after converting to colum major order */
void LoadIdentity(); /* loads the identity matrix into m */
void Rotate(float,float,float,float);
void Scale(float,float,float);
void Translate(float,float,float);
void MultMatrix(float *_m);
void PushMatrix();
void PopMatrix();

/* compute v1 X v2 and store it in v3 */
void CrossProduct(float *v1, float *v2, float *v3);
void Normalize(float *v);

#endif
