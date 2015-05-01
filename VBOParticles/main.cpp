#include "GL\glew.h"
#include "GL\freeglut.h"
#include <iostream>

GLuint vbo;
GLfloat interleavedData[] = { 0, 2, -4, 1, 0, 0, -2, -2, -4, 0, 1, 0, 2, -2, -4, 0, 0, 1 };
GLuint vertexShader, fragmentShader, shaderProgram;

void makeShaders(){
	const char *vsText, *fsText;

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShader, 1, &vsText, NULL);
	glCompileShader(vertexShader);
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);
}

void init(){

	glClearColor(0, 0, 0, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50, 640.0 / 480.0, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);

	//create identifiers
	glGenBuffers(1, &vbo);
	// Bind identifier
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(interleavedData), interleavedData, GL_STATIC_DRAW); // GL_DYNAMIC for changing data.

}

void display(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Have to bind buffer in display
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//GlVertexPointer code (I believe its outdated)
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 6 * sizeof(GLfloat), 0);
	glColorPointer(3, GL_FLOAT, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0); //bind nothing

	//With VertexAttribPointer instead of vertex-/color pointer
	/*glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
	glBindVertexArray(vbo);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);*/



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

	glutMainLoop();

	return 0;
}