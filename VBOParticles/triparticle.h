#ifndef TRI_PARTICLE
#define TRI_PARTICLE

#include "vertex.h"
#include "color.h"

#define LIFE_TIME (300)

static const vertex MIN_START{ -3.0, 2.0, 0.0 };
static const vertex MAX_START { 2.0, 2.0, 0.0 };

struct Particle {

	vertex position;
	vertex velocity;

	GLfloat rotation;
	GLfloat velZ;

	color color;

	int lifetime;
};

void init_random_triparticle(
	Particle *t,
	vertex minPos = MIN_START,
	vertex maxPos = MAX_START) {

	// Random position between min and max
	t->position = vertex_random(
		minPos.x, maxPos.x,
		minPos.y, maxPos.y,
		minPos.z, maxPos.z);

	// Speed is not random yet, just some downwards direction for now 

	t->velocity = {
		0.0,
		randFloatRange(-0.03, -0.001),
		0.0
	};

	t->rotation = 0.0f;
	t->velZ = randFloatRange(-0.02, 0.02);

	t->lifetime = LIFE_TIME;

	t->color = color_random(0.2);

}

void update_triparticle(Particle* t) {
	
	t->lifetime--;

	t->position = vertex_add(t->position, t->velocity);

	t->rotation += t->velZ;
}

#endif
