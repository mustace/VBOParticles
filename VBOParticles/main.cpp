#include "GL\glew.h"
#include "GL\freeglut.h"
#include <iostream>
#include "pool.h"
#include "shaders.h"
#include "vertex.h"
#include "triparticle.h"
#include "obj.h"

#define MAX_PARTICLES (32)
#define LINE_SIZE (256)
#define FRAME_MSEC (17)
#define EMIT_FRAME_DELAY (2)
#define EMIT_AMOUNT (5)

#define POSITION_COORDINATES (3)
#define NORMAL_COORDINATES (3)
#define COLOR_COORDINATES (4)
#define TRANSLATION_COORDINATES (3)
#define ROTZ_COORDINATES (1)

GLuint colorBufferObject, translationBufferObject, rotZBufferObject;

GLuint instanceVertexBufferObject, instanceNormalBufferObject;

GLfloat vertexColors[MAX_PARTICLES * COLOR_COORDINATES]; // RGBA format
GLfloat vertexTranslations[MAX_PARTICLES * TRANSLATION_COORDINATES]; // XYZ format
GLfloat vertexRotations[MAX_PARTICLES * ROTZ_COORDINATES]; // float format

GLfloat* instanceVertices;
GLfloat* instanceNormals;
int instanceVertexCount;

GLuint shaderProgram;
GLint attribute_color;
GLint attribute_translation;
GLint attribute_rotz;
GLint attribute_normal;

Pool<Particle> pool;


void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 033:
	case 'q':
		exit(1);
		break;
	}
}

void drawInBuffer(Particle* t, int index) {
	
	color* colorBuffer = (color*)(vertexColors + index * COLOR_COORDINATES);
	vertex* translationBuffer = (vertex*)(vertexTranslations + index * TRANSLATION_COORDINATES);
	GLfloat* rotzBuffer = vertexRotations + index * ROTZ_COORDINATES ;

	*colorBuffer = t->color;
	*translationBuffer = t->position;
	*rotzBuffer = t->rotZ;

}

// Draw callback 
void display(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Load in new data to the buffers
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColors), vertexColors, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, translationBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexTranslations), vertexTranslations, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, rotZBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexRotations), vertexRotations, GL_DYNAMIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0); // or null, whatever we feel like

	// Specify how the shader finds data in the buffers

	//glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject); // Sets active buffer
	// =>
	glBindBuffer(GL_ARRAY_BUFFER, instanceVertexBufferObject);

	glEnableVertexAttribArray(0); // Enables an attribute slot
	
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL); // Specifies how to parse vertexarrayobject data into an attribute
	// =>
	glVertexAttribPointer(
		0, // attribute. No particular reason for 0, but must match the layout in the shader.
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		NULL // array buffer offset
	);

	glBindBuffer(GL_ARRAY_BUFFER, instanceNormalBufferObject);
	glEnableVertexAttribArray(attribute_normal);
	glVertexAttribPointer(
		attribute_normal, // attribute. No particular reason for 0, but must match the layout in the shader.
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		NULL // array buffer offset
	);


	glBindBuffer(GL_ARRAY_BUFFER, colorBufferObject);
	glEnableVertexAttribArray(attribute_color);
	glVertexAttribPointer(attribute_color, 4, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, translationBufferObject);
	glEnableVertexAttribArray(attribute_translation);
	glVertexAttribPointer(attribute_translation, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, rotZBufferObject);
	glEnableVertexAttribArray(attribute_rotz);
	glVertexAttribPointer(attribute_rotz, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	
	glVertexAttribDivisor(0, 0); 
	glVertexAttribDivisor(attribute_normal, 0); 
	glVertexAttribDivisor(attribute_color, 1); 
	glVertexAttribDivisor(attribute_translation, 1); 
	glVertexAttribDivisor(attribute_rotz, 1); 

	glDrawArraysInstanced(GL_TRIANGLES, 0, instanceVertexCount, pool.count());

	
	//glDrawArrays(GL_TRIANGLES, 0, pool.count() * 3); // 3 vertices for each tri

	// Reset the holy global state machine that is openGL
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(attribute_normal);
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

			if (pool.count() >= MAX_PARTICLES)
				break;

			Particle* t = pool.new_object();

			init_random_triparticle(t);

		}
	}

	for (int i = 0; i < pool.count(); ++i) {
		Particle* t = pool.at(i);

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
	gluPerspective(40.0, 1.0, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0.0, 0.0, 15.0,  // Set eye position, target position, and up direction.
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT /* or GL_BACK or even GL_FRONT_AND_BACK */);
	glFrontFace(GL_CW /* or GL_CCW */);



	// Prepare the shader program.
	GLuint shaderProgram = LoadShaderProgram();
	glUseProgram(shaderProgram);

	// generate "pointers" (names) for each buffer
	glGenBuffers(1, &colorBufferObject);
	glGenBuffers(1, &translationBufferObject);
	glGenBuffers(1, &rotZBufferObject);
	// NEW INSTANCE BUFFER
	glGenBuffers(1, &instanceVertexBufferObject);
	glGenBuffers(1, &instanceNormalBufferObject);
	
	// put data in buffers - glBindBuffer sets the active buffer, glBufferData pours data in the active buffer
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColors), vertexColors, GL_DYNAMIC_DRAW); 
	glBindBuffer(GL_ARRAY_BUFFER, translationBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexTranslations), vertexTranslations, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, rotZBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexRotations), vertexRotations, GL_DYNAMIC_DRAW);
	// NEW INSTANCE BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, instanceVertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * instanceVertexCount * 3, instanceVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, instanceNormalBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * instanceVertexCount * 3, instanceNormals, GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, 0); // or null, whatever we feel like

	attribute_color = glGetAttribLocation(shaderProgram, attribute_name_color);
	if (attribute_color == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name_color);
	}
	else
	{
		printf("%s bound to %d\n", attribute_name_color, attribute_color);
	}

	attribute_translation = glGetAttribLocation(shaderProgram, attribute_name_translation);
	if (attribute_translation == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name_translation);
	}
	else
	{
		printf("%s bound to %d\n", attribute_name_translation, attribute_translation);
	}

	attribute_rotz = glGetAttribLocation(shaderProgram, attribute_name_rotation);
	if (attribute_rotz == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name_rotation);
	}
	else
	{
		printf("%s bound to %d\n", attribute_name_rotation, attribute_rotz);
	}

	attribute_normal = glGetAttribLocation(shaderProgram, attribute_name_normal);
	if (attribute_normal == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name_normal);
	}
	else
	{
		printf("%s bound to %d\n", attribute_name_normal, attribute_normal);
	}

}

int main(int argc, char **argv) {

	vector<vertex> vertices;
	vector<triangle> triangles;

	read_obj_file("cube.obj", &vertices, &triangles);

	int T = triangles.size();

	instanceVertexCount = T * 3;
	instanceVertices = (GLfloat*)malloc((sizeof(GLfloat)) * 3 * instanceVertexCount);
	instanceNormals = (GLfloat*)malloc((sizeof(GLfloat)) * 3 * instanceVertexCount);

	vertex* verts = (vertex*)instanceVertices;
	vertex* norms = (vertex*)instanceNormals;

	for (int i = 0; i < T; ++i)
	{
		triangle t = triangles[i];
		vertex v1 = vertices[t.i1];
		vertex v2 = vertices[t.i2];
		vertex v3 = vertices[t.i3];
		verts[i * 3 + 0] = v1;
		verts[i * 3 + 1] = v2;
		verts[i * 3 + 2] = v3;
		vertex n = vertex_normal(v1, v2, v3);
		norms[i * 3 + 0] = n;
		norms[i * 3 + 1] = n;
		norms[i * 3 + 2] = n;

	}

	for (int i = 0; i < (T * 3 * 3); ++i)
	{
		instanceVertices[i] *= 0.5;
	}


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