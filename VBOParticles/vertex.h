#ifndef VERTEX
#define VERTEX

#include <GL/glew.h>

struct color {
	GLfloat r;
	GLfloat g;
	GLfloat b;
	GLfloat a;
};

struct vertex {
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

vertex vertex_normal(vertex* vs);

vertex vertex_add(vertex a, vertex b);

#endif