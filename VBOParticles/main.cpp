#include "GL\glew.h"
#include "GL\freeglut.h"
#include <iostream>
#include "pool.h"
#include "shaders.h"
#include "vertex.h"
#include "triparticle.h"
#include "obj.h"
#include <glm/mat4x4.hpp>
#include "SOIL.h"

#define MAX_PARTICLES (1000)
#define LINE_SIZE (256)
#define FRAME_MSEC (17)
#define EMIT_FRAME_DELAY (20)
#define EMIT_AMOUNT (2)
#define REFLECTION_RATE (1.0)

#define POSITION_COORDINATES (3)
#define NORMAL_COORDINATES (3)
#define COLOR_COORDINATES (4)
#define TRANSLATION_COORDINATES (3)
#define ROTATION_COORDINATES (1)

GLuint 
	vboIdVertex, 
	vboIdNormal, 
	vboIdColor, 
	vboIdTranslation, 
	vboIdRotation,
	vboIdScale;

GLint
	attributeIdVertex,
	attributeIdNormal,
	attributeIdColor,
	attributeIdTranslation,
	attributeIdRotation,
	uniformIdScale,
	uniformIdReflecRate;

GLfloat particleColors[MAX_PARTICLES * COLOR_COORDINATES]; // RGBA format
GLfloat particleTranslations[MAX_PARTICLES * TRANSLATION_COORDINATES]; // XYZ format
GLfloat particleRotations[MAX_PARTICLES * ROTATION_COORDINATES]; // float format
GLfloat particleScales[] = {
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.0, 0.0, 0.0, 1.0
}; // 4x4mat format

GLfloat* instanceVertices;
GLfloat* instanceNormals;
int instanceVertexCount;

glm::mat4 proj;

GLuint shaderProgram;
GLint uniform_campos;

GLuint cubemapID;

char* textureStrings[6] = {
	"posx.jpg",
	"negx.jpg",
	"posy.jpg",
	"negy.jpg",
	"posz.jpg",
	"negz.jpg"
};


Pool<Particle> pool;

GLfloat cameraPosition[3] = { 0.0, 0.0, 10.0 };

void keyboard(unsigned char key, int x, int y) {

	switch (key) {
	case 033:
		exit(1);
		break;
	case 'a':
		particleScales[12] += 0.2;
		break;
	case 'd':
		particleScales[12] -= 0.2;
		break;
	case 'w':
		particleScales[14] += 0.2;
		break;
	case 's':
		particleScales[14] -= 0.2;
		break;
	case 'q':
		particleScales[13] -= 0.2;
		break;
	case 'e':
		particleScales[13] += 0.2;
		break;

	}

}

void drawInBuffer(Particle* t, int index) {
	
	color* colorBuffer = (color*)(particleColors + index * COLOR_COORDINATES);
	vertex* translationBuffer = (vertex*)(particleTranslations + index * TRANSLATION_COORDINATES);
	GLfloat* rotationBuffer = particleRotations + index * ROTATION_COORDINATES;

	*colorBuffer = t->color;
	*translationBuffer = t->position;
	*rotationBuffer = t->rotation;

}

// Draw callback 
void display(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniform3fv(uniform_campos, 1, cameraPosition);
	glUniform1f(uniformIdReflecRate, REFLECTION_RATE);

	// Load in new data to the buffers
	glBindBuffer(GL_ARRAY_BUFFER, vboIdColor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particleColors), particleColors, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vboIdTranslation);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particleTranslations), particleTranslations, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vboIdRotation);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particleRotations), particleRotations, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // or null, whatever we feel like

	// Specify how the shader finds data in the buffers

	//glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject); // Sets active buffer
	// =>
	glBindBuffer(GL_ARRAY_BUFFER, vboIdVertex);

	glEnableVertexAttribArray(attributeIdVertex); // Enables an attribute slot
	
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL); // Specifies how to parse vertexarrayobject data into an attribute
	// =>
	glVertexAttribPointer(
		attributeIdVertex, // attribute. No particular reason for 0, but must match the layout in the shader.
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		NULL // array buffer offset
	);

	glBindBuffer(GL_ARRAY_BUFFER, vboIdNormal);
	glEnableVertexAttribArray(attributeIdNormal);
	glVertexAttribPointer(
		attributeIdNormal, // attribute. No particular reason for 0, but must match the layout in the shader.
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		NULL // array buffer offset
	);


	glBindBuffer(GL_ARRAY_BUFFER, vboIdColor);
	glEnableVertexAttribArray(attributeIdColor);
	glVertexAttribPointer(attributeIdColor, 4, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, vboIdTranslation);
	glEnableVertexAttribArray(attributeIdTranslation);
	glVertexAttribPointer(attributeIdTranslation, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, vboIdRotation);
	glEnableVertexAttribArray(attributeIdRotation);
	glVertexAttribPointer(attributeIdRotation, 1, GL_FLOAT, GL_FALSE, 0, NULL);

	glVertexAttribDivisor(attributeIdVertex, 0); 
	glVertexAttribDivisor(attributeIdNormal, 0); 
	glVertexAttribDivisor(attributeIdColor, 1); 
	glVertexAttribDivisor(attributeIdTranslation, 1); 
	glVertexAttribDivisor(attributeIdRotation, 1); 

	glDrawArraysInstanced(GL_TRIANGLES, 0, instanceVertexCount, pool.count());

	
	//glDrawArrays(GL_TRIANGLES, 0, pool.count() * 3); // 3 vertices for each tri

	// Reset the holy global state machine that is openGL
	glDisableVertexAttribArray(attributeIdVertex);
	glDisableVertexAttribArray(attributeIdNormal);
	glDisableVertexAttribArray(attributeIdColor);
	glDisableVertexAttribArray(attributeIdTranslation);
	glDisableVertexAttribArray(attributeIdRotation);

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

void loadCubemap() {
	glGenTextures(1, &cubemapID);
	glActiveTexture(GL_TEXTURE0);

	int width, height;
	unsigned char* image;
	for (GLuint i = 0; i < 6; i++)
	{
		image = SOIL_load_image(textureStrings[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
			);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

// Setup before we run
void init(){
	// Load in cubemap
	loadCubemap();

	glClearColor(0, 0, 0, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, 1.0, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);

	gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2],  // Set eye position, target position, and up direction.
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
	glGenBuffers(1, &vboIdVertex);
	glGenBuffers(1, &vboIdNormal); 
	glGenBuffers(1, &vboIdColor);
	glGenBuffers(1, &vboIdTranslation);
	glGenBuffers(1, &vboIdRotation);
	glGenBuffers(1, &vboIdScale);
	
	
	// put data in buffers - glBindBuffer sets the active buffer, glBufferData pours data in the active buffer
	glBindBuffer(GL_ARRAY_BUFFER, vboIdVertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * instanceVertexCount * 3, instanceVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vboIdNormal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * instanceVertexCount * 3, instanceNormals, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vboIdColor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particleColors), particleColors, GL_DYNAMIC_DRAW); 
	glBindBuffer(GL_ARRAY_BUFFER, vboIdTranslation);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particleTranslations), particleTranslations, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vboIdRotation);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particleRotations), particleRotations, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vboIdScale);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particleScales), particleScales, GL_DYNAMIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0); // or null, whatever we feel like

	attributeIdVertex = glGetAttribLocation(shaderProgram, attributeNameVertex);
	if (attributeIdVertex == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attributeNameVertex);
	}
	else
	{
		printf("%s bound to %d\n", attributeNameVertex, attributeIdVertex);
	}

	uniform_campos= glGetUniformLocation(shaderProgram, uniform_name_campos);
	if (uniform_campos == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniform_name_campos);
	}
	else
	{
		printf("%s bound to %d\n", uniform_name_campos, uniform_campos);
	}

	uniformIdReflecRate = glGetUniformLocation(shaderProgram, uniformNameReflecRate);
	if (uniformIdReflecRate == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniformNameReflecRate);
	}
	else
	{
		printf("%s bound to %d\n", uniformNameReflecRate, uniformIdReflecRate);
	}

	attributeIdNormal = glGetAttribLocation(shaderProgram, attributeNameNormal);
	if (attributeIdNormal == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attributeNameNormal);
	}
	else
	{
		printf("%s bound to %d\n", attributeNameNormal, attributeIdNormal);
	}

	attributeIdColor = glGetAttribLocation(shaderProgram, attributeNameColor);
	if (attributeIdColor == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attributeNameColor);
	}
	else
	{
		printf("%s bound to %d\n", attributeNameColor, attributeIdColor);
	}

	attributeIdTranslation = glGetAttribLocation(shaderProgram, attributeNameTranslation);
	if (attributeIdTranslation == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attributeNameTranslation);
	}
	else
	{
		printf("%s bound to %d\n", attributeNameTranslation, attributeIdTranslation);
	}

	attributeIdRotation = glGetAttribLocation(shaderProgram, attributeNameRotation);
	if (attributeIdRotation == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attributeNameRotation);
	}
	else
	{
		printf("%s bound to %d\n", attributeNameRotation, attributeIdRotation);
	}

	uniformIdScale = glGetUniformLocation(shaderProgram, uniformNameScale);
	if (uniformIdScale == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", uniformNameScale);
	}
	else
	{
		printf("%s bound to %d\n", uniformNameScale, uniformIdScale);
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

	for (int i = 0; i < T * 9; ++i) {

		instanceVertices[i] *= 0.5f;

	}

	/*GLuint tex_cube = SOIL_load_OGL_cubemap
		(
		"posx.jpg",
		"negx.jpg",
		"posy.jpg",
		"negy.jpg",
		"posz.jpg",
		"negz.jpg",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS
		);*/


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