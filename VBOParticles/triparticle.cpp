#include "GL\glew.h"
#include "GL\freeglut.h"
#include "vertex.h"
#include "triparticle.h"

GLfloat randFloatRange(GLfloat M, GLfloat N)
{
	return M + (rand() / (RAND_MAX / (N - M)));
}

// Returns random particle, xyz within same range
vertex make_random_vertex(GLfloat min, GLfloat max) {
	return{
		randFloatRange(min, max),
		randFloatRange(min, max),
		randFloatRange(min, max)
	};
}

// Returns random particle, xyz seperate ranges
vertex make_random_vertex(GLfloat xMin, GLfloat xMax,
	GLfloat yMin, GLfloat yMax,
	GLfloat zMin, GLfloat zMax) {
	return{
		randFloatRange(xMin, xMax),
		randFloatRange(yMin, yMax),
		randFloatRange(zMin, zMax)
	};
}

color make_random_color() {
	return{
		randFloatRange(0.0, 1.0),
		randFloatRange(0.0, 1.0),
		randFloatRange(0.0, 1.0),
		randFloatRange(0.0, 1.0)
	};
}

void init_random_triparticle(Triparticle *t,
	GLfloat minSize,
	GLfloat maxSize,
	vertex minPos,
	vertex maxPos) {

	// Random position between min and max
	t->pos = make_random_vertex(
		minPos.x, maxPos.x,
		minPos.y, maxPos.y,
		minPos.z, maxPos.z);

	t->v1 = { 1.0, 0.0, 0.0 };
	t->v2 = { -1.0, 0.0, 0.0 };
	t->v3 = { 0.0, 1.0, 0.0 };

	/*
	t->v1 = make_random_vertex(minSize, maxSize);
	t->v2 = make_random_vertex(minSize, maxSize);
	t->v3 = make_random_vertex(minSize, maxSize);
	*/

	// Speed is not random yet, just some downwards direction for now 

	t->velocity = {
		0.0,
		randFloatRange(-0.03, -0.001),
		0.0
	};

	t->rotZ = 0.0f;
	t->velZ = 0.01f;

	t->lifetime = LIFE_TIME;

	t->color = make_random_color();

}

void update_triparticle(Triparticle* t) {
	vertex newPos = t->pos;

	newPos.x += t->velocity.x;
	newPos.y += t->velocity.y;
	newPos.z += t->velocity.z;

	t->lifetime--;

	t->pos = newPos;

	t->rotZ += t->velZ;
}
