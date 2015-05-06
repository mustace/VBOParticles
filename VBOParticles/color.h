#ifndef COLOR
#define COLOR

#include <GL/glew.h>
#include "random.h"

struct color {
	GLfloat r;
	GLfloat g;
	GLfloat b;
	GLfloat a;
};

color color_random(GLfloat min = 0.0, bool randomAlpha = false) {
	return{
		randFloatRange(min, 1.0),
		randFloatRange(min, 1.0),
		randFloatRange(min, 1.0),
		randomAlpha ? randFloatRange(min, 1.0) : 1.0
	};
}

#endif