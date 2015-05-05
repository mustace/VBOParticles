#include "GL\glew.h"
#include "GL\freeglut.h"
#include <iostream>
#include "particles.h"
#include "pool.h"
#include "shaders.h"
#include "keyboard.h"

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
#define TRANSLATION_COORDINATES (3)
#define ROTZ_COORDINATES (1)
#include "utils.h"

GLuint vertexBufferObject, normalBufferObject, colorBufferObject, translationBufferObject, rotZBufferObject;

GLfloat interleavedData[] = { 0, 2, -4, 1, 0, 0, -2, -2, -4, 0, 1, 0, 2, -2, -4, 0, 0, 1 };

GLfloat vertexPositions[MAX_PARTICLES * POSITION_COORDINATES]; // XYZ format
GLfloat vertexNormals[MAX_PARTICLES* NORMAL_COORDINATES]; // not used atm
GLfloat vertexColors[MAX_PARTICLES * COLOR_COORDINATES]; // RGBA format
GLfloat vertexTranslations[MAX_PARTICLES * TRANSLATION_COORDINATES]; // XYZ format
GLfloat vertexRotZs[MAX_PARTICLES * ROTZ_COORDINATES]; // float format

GLuint shaderProgram;
GLint attribute_color;
GLint attribute_translation;
GLint attribute_rotz;

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

	t->v1 = make_random_vertex(minSize, maxSize);
	t->v2 = make_random_vertex(minSize, maxSize);
	t->v3 = make_random_vertex(minSize, maxSize);

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


void drawInBuffer(Triparticle* t, int index) {
	// tri verts in world space
	vertex ps[3];
	// find world coords for verts

	ps[0] = t->v1;
	ps[1] = t->v2;
	ps[2] = t->v3;
	
	vertex n = vertex_normal(ps);

	vertex* posBuffer = (vertex*) (vertexPositions + index * POSITION_COORDINATES * 3); // 3 verts to a particle
	vertex* normalBuffer = (vertex*) (vertexNormals + index * NORMAL_COORDINATES * 3);
	color* colorBuffer = (color*)(vertexColors + index *COLOR_COORDINATES * 3);
	vertex* translationBuffer = (vertex*)(vertexTranslations + index * TRANSLATION_COORDINATES * 3);
	GLfloat* rotzBuffer = vertexRotZs + index * ROTZ_COORDINATES * 3;

	for (int i = 0; i < 3; ++i) { 

		posBuffer[i] = ps[i];
		normalBuffer[i] = n;
		colorBuffer[i] = t->color;
		translationBuffer[i] = t->pos;
		rotzBuffer[i] = t->rotZ;
		
	}

}

// Draw callback 
void display(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Load in new data to the buffers
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_DYNAMIC_DRAW); // GL_DYNAMIC for changing data.
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexNormals), vertexNormals, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColors), vertexColors, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, translationBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexTranslations), vertexTranslations, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, rotZBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexRotZs), vertexRotZs, GL_DYNAMIC_DRAW);
	
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

	glBindBuffer(GL_ARRAY_BUFFER, translationBufferObject);
	glEnableVertexAttribArray(attribute_translation);
	glVertexAttribPointer(attribute_translation, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, rotZBufferObject);
	glEnableVertexAttribArray(attribute_rotz);
	glVertexAttribPointer(attribute_rotz, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	
	// Draw from the specified part of the array
	glDrawArrays(GL_TRIANGLES, 0, pool.count() * 3); // 3 vertices for each tri

	// Reset the holy global state machine that is openGL
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(attribute_color);
	glDisableVertexAttribArray(attribute_translation);
	glDisableVertexAttribArray(attribute_rotz);

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

		// Draw the triparticle to the buffer arrays
		drawInBuffer(t, i);
	}

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

	// Prepare the shader program.
	GLuint shaderProgram = LoadShaderProgram();
	glUseProgram(shaderProgram);

	// generate "pointers" (names) for each buffer
	glGenBuffers(1, &vertexBufferObject);
	glGenBuffers(1, &normalBufferObject);
	glGenBuffers(1, &colorBufferObject);
	glGenBuffers(1, &translationBufferObject);
	glGenBuffers(1, &rotZBufferObject);
	
	// put data in buffers - glBindBuffer sets the active buffer, glBufferData pours data in the active buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_DYNAMIC_DRAW); // GL_DYNAMIC for changing data.
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexNormals), vertexNormals, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColors), vertexColors, GL_DYNAMIC_DRAW); 
	glBindBuffer(GL_ARRAY_BUFFER, translationBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexTranslations), vertexTranslations, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, rotZBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexRotZs), vertexRotZs, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // or null, whatever we feel like

	attribute_color = glGetAttribLocation(shaderProgram, attribute_color_name);
	if (attribute_color == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_color_name);
	}

	attribute_translation = glGetAttribLocation(shaderProgram, attribute_translation_name);
	if (attribute_translation == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_translation_name);
	}

	attribute_rotz = glGetAttribLocation(shaderProgram, attribut_rotz_name);
	if (attribute_rotz == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribut_rotz_name);
	}

}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(512, 128);//optional
	glutInitWindowSize(800, 600); //optional
	glutCreateWindow("Particle Window!");
	glewInit();

	init();

	// register callbacks
	glutKeyboardFunc(keyboard);
	glutTimerFunc(FRAME_MSEC, onFrame, 0);
	//glutDisplayFunc(display);

	printf("OpenGL version supported by this platform (%s): \n", (char const*) glGetString(GL_VERSION));

	glutMainLoop();

	return 0;
}