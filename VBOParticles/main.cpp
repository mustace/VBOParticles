#include "GL\glew.h"
#include "GL\freeglut.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

GLuint vertexBufferObject, normalBufferObject, colorBufferObject;

GLfloat interleavedData[] = { 0, 2, -4, 1, 0, 0, -2, -2, -4, 0, 1, 0, 2, -2, -4, 0, 0, 1 };

GLfloat vertexPositions[] = { 0, 2, -4, -2, -2, -4, 2, -2, -4 }; // XYZ format
GLfloat vertexNormals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // not used atm
GLfloat vertexColors[] = { 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1 }; // RGBA format

GLuint vertexShader, fragmentShader, shaderProgram;


//  http://www.nexcius.net/2012/11/20/how-to-load-a-glsl-shader-in-opengl-using-c/ readfile method
std::string readFile(const char *filePath) {
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if (!fileStream.is_open()) {
		std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

// Loads a shader from files.
GLuint LoadShader(const char *vertex_path, const char *fragment_path) {
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Read shaders
	std::string vertShaderStr = readFile(vertex_path);
	std::string fragShaderStr = readFile(fragment_path);
	const char *vertShaderSrc = vertShaderStr.c_str();
	const char *fragShaderSrc = fragShaderStr.c_str();

	glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
	glCompileShader(vertShader);
	glCompileShader(fragShader);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);
	return program;
}


// Setup before we run
void init(){

	glClearColor(0, 0, 0, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50, 640.0 / 480.0, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);

	GLuint shaderProgram = LoadShader("vertex.glsl", "fragment.glsl");
	glUseProgram(shaderProgram);

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

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, normalBufferObject);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, colorBufferObject);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, NULL);

	// Draw from the specified part of the array
	//glBindVertexArray(vbo);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	//glBindVertexArray(0);

	// Reset the holy global state machine that is openGL
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glutSwapBuffers();
}

int main(int argc, char **argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(500, 500);//optional
	glutInitWindowSize(800, 600); //optional
	glutCreateWindow("Particle Window!");
	glewInit();

	init();

	// register callbacks
	glutDisplayFunc(display);
	glutIdleFunc(display);

	printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));

	glutMainLoop();

	return 0;
}