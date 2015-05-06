#include "vertex.h"

struct Quad {
	vertex pos;
	vertex v1, v2, v3;
	// Maybe have rotation and angular velocity?  
	vertex velocity;

	GLfloat rotZ;
	GLfloat velZ;

	color color;
	int lifetime;
};