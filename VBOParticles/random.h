#ifndef RANDOM
#define RANDOM

#include <GL/glew.h>
#include <GL/freeglut.h>

inline GLfloat randFloatRange(GLfloat M, GLfloat N)
{
	return M + (rand() / (RAND_MAX / (N - M)));
}

#endif