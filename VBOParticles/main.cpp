#include "GL\glew.h"
#include "GL\freeglut.h"
#include <iostream>
#include "particles.h"
#include "pool.h"
#include "shaders.h"

#define MAX_PARTICLES (10000)
#define LINE_SIZE (256)
#define FRAME_MSEC (17)
#define EMIT_FRAME_DELAY (2)
#define EMIT_AMOUNT (5)
#define MIN_TRI_SIZE (0.3)
#define MAX_TRI_SIZE (0.8)
#define LIFE_TIME (300)

#define POSITION_COORDINATES (3)
#define NORMAL_COORDINATES (3)
#define COLOR_COORDINATES (4)

GLuint vertexBufferObject, normalBufferObject, colorBufferObject;

GLfloat interleavedData[] = { 0, 2, -4, 1, 0, 0, -2, -2, -4, 0, 1, 0, 2, -2, -4, 0, 0, 1 };

GLfloat vertexPositions[MAX_PARTICLES * POSITION_COORDINATES]; // XYZ format
GLfloat vertexNormals[MAX_PARTICLES* NORMAL_COORDINATES]; // not used atm
GLfloat vertexColors[MAX_PARTICLES * COLOR_COORDINATES]; // RGBA format

GLuint shaderProgram;

char* attribute_color_name = "inColor";
GLint attribute_color;

Pool<Triparticle> pool;

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

double randFloatRange(double M, double N)
{
	return M + (rand() / (RAND_MAX / (N - M)));
}

// Returns random particle, xyz within same range
vertex make_random_vertex(double min, double max) {
	return{
		randFloatRange(min, max),
		randFloatRange(min, max),
		randFloatRange(min, max)
	};
}

// Returns random particle, xyz seperate ranges
vertex make_random_vertex(double xMin, double xMax,
	double yMin, double yMax,
	double zMin, double zMax) {
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
	double minSize,
	double maxSize,
	vertex minPos,
	vertex maxPos) {

	// Random position between min and max
	t->pos = make_random_vertex(
		minPos.x, maxPos.x,
		minPos.y, maxPos.y,
		minPos.z, maxPos.z);

	t->v1 = make_random_vertex(minSize, maxSize);
	t->v2 = make_random_vertex(minSize, maxSize);
	t->v3 = make_random_vertex(minSize, maxSize);

	// Speed is not random yet, just some downwards direction for now 

	t->velocity = {
		0.0,
		randFloatRange(-0.03, -0.001),
		0.0
	};

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
}


void drawInBuffer(Triparticle* t, int index) {
	// tri verts in world space
	vertex v1, v2, v3;
	// edges
	vertex e1, e2;
	// normal
	vertex n;

	// find world coords for verts
	v1.x = t->pos.x + t->v1.x;
	v1.y = t->pos.y + t->v1.y;
	v1.z = t->pos.z + t->v1.z;

	v2.x = t->pos.x + t->v2.x;
	v2.y = t->pos.y + t->v2.y;
	v2.z = t->pos.z + t->v2.z;

	v3.x = t->pos.x + t->v3.x;
	v3.y = t->pos.y + t->v3.y;
	v3.z = t->pos.z + t->v3.z;


	// e1 is edge from v1 to v2.
	e1.x = v2.x - v1.x;
	e1.y = v2.y - v1.y;
	e1.z = v2.z - v1.z;

	// e2 is edge from v1 to v3.
	e2.x = v3.x - v1.x;
	e2.y = v3.y - v1.y;
	e2.z = v3.z - v1.z;

	// normal is e1 x e2.  (Note: Does not need to be unit length for glNormal.)
	n.x = (e1.y * e2.z) - (e1.z * e2.y);
	n.y = (e1.z * e2.x) - (e1.x * e2.z);
	n.z = (e1.x * e2.y) - (e1.y * e2.x);


	GLfloat* posBuffer = vertexPositions + index * POSITION_COORDINATES * 3; // 3 verts to a particle

	posBuffer[0] = v1.x;
	posBuffer[1] = v1.y;
	posBuffer[2] = v1.z;
	posBuffer[3] = v2.x;
	posBuffer[4] = v2.y;
	posBuffer[5] = v2.z;
	posBuffer[6] = v3.x;
	posBuffer[7] = v3.y;
	posBuffer[8] = v3.z;

	GLfloat* normalBuffer = vertexNormals + index * NORMAL_COORDINATES * 3; 

	normalBuffer[0] = n.x;
	normalBuffer[1] = n.y;
	normalBuffer[2] = n.z;
	normalBuffer[3] = n.x;
	normalBuffer[4] = n.y;
	normalBuffer[5] = n.z;
	normalBuffer[6] = n.x;
	normalBuffer[7] = n.y;
	normalBuffer[8] = n.z;

	//TODO Do colors as well later!
	GLfloat* colorBuffer = vertexColors + index *COLOR_COORDINATES * 3;

	colorBuffer[0] = t->color.r;
	colorBuffer[1] = t->color.g;
	colorBuffer[2] = t->color.b;
	colorBuffer[3] = t->color.a;
	colorBuffer[4] = t->color.r;
	colorBuffer[5] = t->color.g;
	colorBuffer[6] = t->color.b;
	colorBuffer[7] = t->color.a;
	colorBuffer[8] = t->color.r;
	colorBuffer[9] = t->color.g;
	colorBuffer[10] = t->color.b;
	colorBuffer[11] = t->color.a;
}


// Draw callback 
void display(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	/*
	//Have to bind buffer in display
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//GlVertexPointer code (outdated)
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 6 * sizeof(GLfloat), 0);
	glColorPointer(3, GL_FLOAT, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0); //bind nothing
	*/

	/*
	// Enable two parameters for the shader
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Specifies how to find the parameters in the VertexArrayObject

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
	*/

	//With VertexAttribPointer instead of vertex-/color pointer

	// Load in new data to the buffers
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_DYNAMIC_DRAW); // GL_DYNAMIC for changing data.
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexNormals), vertexNormals, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColors), vertexColors, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0); // or null, whatever we feel like


	// Specify how the shader finds data in the buffers
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject); // Sets active buffer
	glEnableVertexAttribArray(0); // Enables an attribute slot
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL); // Specifies how to parse vertexarrayobject data into an attribute

	glBindBuffer(GL_ARRAY_BUFFER, normalBufferObject);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, colorBufferObject);
	glEnableVertexAttribArray(attribute_color);
	glVertexAttribPointer(attribute_color, 4, GL_FLOAT, GL_FALSE, 0, NULL);

	// Draw from the specified part of the array
	//glBindVertexArray(vbo);
	glDrawArrays(GL_TRIANGLES, 0, pool.count() * 3); // 3 vertices for each tri
	//glBindVertexArray(0);

	// Reset the holy global state machine that is openGL
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(attribute_color);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glutSwapBuffers();
}


// Function runs at set interval, (a onFrame function, of sorts)
void onFrame(int value) {
	// emitter stuff
	static int count = 0;
	count++;
	if (count % EMIT_FRAME_DELAY == 0) {
		for (int i = 0; i < EMIT_AMOUNT; i++) {

			Triparticle* t = pool.new_object();

			init_random_triparticle(t, MIN_TRI_SIZE, MAX_TRI_SIZE, MIN_START, MAX_START);

		}
	}

	for (int i = 0; i < pool.count(); ++i) {
		Triparticle* t = pool.at(i);

		update_triparticle(t);

		if (t->lifetime < 0)
		{
			pool.mark_dead(i--);
			continue;
		}


		// draw_triparticle(t);

		// Draw the triparticle to the buffer arrays
		drawInBuffer(t, i);
	}

	//glRotatef(3, 0.0, 1.0, 0.0);

	display();

	// Call this func again after delay
	glutTimerFunc(FRAME_MSEC, onFrame, 0);
}



// Setup before we run
void init(){

	glClearColor(0, 0, 0, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, 1.0, 1.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0.0, 0.0, 5.0,  // Set eye position, target position, and up direction.
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.);
	glEnable(GL_DEPTH_TEST);

	// Specif what shader programs to use

	// glBindAttribLocation(program, 2, attribute_color_name);

	GLuint shaderProgram = LoadProgram("vertex.glsl", "fragment.glsl");
	glUseProgram(shaderProgram);

	// initArrays(MAX_PARTICLES);

	// generate "pointers" (names) for each buffer
	glGenBuffers(1, &vertexBufferObject);
	glGenBuffers(1, &normalBufferObject);
	glGenBuffers(1, &colorBufferObject);


	// put data in buffers - glBindBuffer sets the active buffer, glBufferData pours data in the active buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_DYNAMIC_DRAW); // GL_DYNAMIC for changing data.
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexNormals), vertexNormals, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColors), vertexColors, GL_DYNAMIC_DRAW); 
	glBindBuffer(GL_ARRAY_BUFFER, 0); // or null, whatever we feel like

	attribute_color = glGetAttribLocation(shaderProgram, attribute_color_name);
	if (attribute_color == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_color_name);
	}
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(500, 500);//optional
	glutInitWindowSize(800, 600); //optional
	glutCreateWindow("Particle Window!");
	glewInit();

	init();

	// register callbacks
	glutDisplayFunc(display);
	glutTimerFunc(FRAME_MSEC, onFrame, 0);

	printf("OpenGL version supported by this platform (%s): \n", (char const*) glGetString(GL_VERSION));

	glutMainLoop();

	return 0;
}