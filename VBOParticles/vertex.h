#ifndef VERTEX
#define VERTEX

#include <GL/glew.h>
#include "random.h"

struct vertex {
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

vertex vertex_sub(vertex a, vertex b) {
	return {
		a.x - b.x,
		a.y - b.y,
		a.z - b.z
	};
}

vertex vertex_add(vertex a, vertex b) {
	return {
		a.x + b.x,
		a.y + b.y,
		a.z + b.z
	};
}

vertex vertex_cross(vertex a, vertex b) {
	return {
		(a.y * b.z) - (a.z * b.y),
		(a.z * b.x) - (a.x * b.z),
		(a.x * b.y) - (a.y * b.x)
	};
}

vertex vertex_normal(vertex* vs)
{
	vertex e1 = vertex_sub(vs[1], vs[0]);
	vertex e2 = vertex_sub(vs[2], vs[0]);

	return vertex_cross(e1, e2);
}

vertex vertex_random(GLfloat min, GLfloat max) {

	return {
		randFloatRange(min, max),
		randFloatRange(min, max),
		randFloatRange(min, max)
	};

}

vertex vertex_random(
	GLfloat xMin, GLfloat xMax,
	GLfloat yMin, GLfloat yMax,
	GLfloat zMin, GLfloat zMax) {

	return{
		randFloatRange(xMin, xMax),
		randFloatRange(yMin, yMax),
		randFloatRange(zMin, zMax)
	};
}

#endif