#include <GL/glew.h>
#include <iostream>

char* readFile(const char* filename) {
	// Open the file
	FILE* fp;
	fopen_s(&fp, filename, "r");
	// Move the file pointer to the end of the file and determing the length
	fseek(fp, 0, SEEK_END);
	long file_length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* contents = new char[file_length + 1];
	// zero out memory
	for (int i = 0; i < file_length + 1; i++) {
		contents[i] = 0;
	}
	// Here's the actual read
	fread(contents, 1, file_length, fp);
	// This is how you denote the end of a string in C
	contents[file_length + 1] = '\0';
	fclose(fp);
	return contents;
}

// Loads a shader from a file.
GLuint LoadShader(const char* file_path, GLenum type)
{
	GLuint shader = glCreateShader(type);

	const char* shaderSrc = readFile(file_path);
	glShaderSource(shader, 1, &shaderSrc, NULL);
	glCompileShader(shader);

	return shader;
}

GLuint LoadProgram(const char *vert_path, const char *frag_path) {

	GLuint program = glCreateProgram();

	GLuint vertShader = LoadShader(vert_path, GL_VERTEX_SHADER);
	GLuint fragShader = LoadShader(frag_path, GL_FRAGMENT_SHADER);

	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);
	return program;
}