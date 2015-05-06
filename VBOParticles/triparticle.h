#ifndef TRI_PARTICLE
#define TRI_PARTICLE

#include "vertex.h"

#define MIN_TRI_SIZE (0.3)
#define MAX_TRI_SIZE (0.8)
#define LIFE_TIME (300)

const vertex MIN_START{
	-3.0,
	2.0,
	0.0
};

const vertex MAX_START{
	2.0,
	2.0,
	0.0
};


// Performance posibility: Have vertices in world space instead of local,
// and disregard position entirely when drawing, saving calculations.
struct Triparticle {
	vertex pos;
	vertex v1, v2, v3;
	// Maybe have rotation and angular velocity?  
	vertex velocity;

	GLfloat rotZ;
	GLfloat velZ;

	color color;
	int lifetime;
};

void init_random_triparticle(
	Triparticle *t, GLfloat minSize,
	GLfloat maxSize,
	vertex minPos,
	vertex maxPos);

void update_triparticle(Triparticle* t);

#endif
