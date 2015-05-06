#ifndef SHADERS
#define SHADERS

#include <GL/glew.h>

GLuint LoadShaderProgram(const char *vert_path = "vertex.glsl", const char *frag_path = "fragment.glsl");

char* attributeNameVertex = "gl_Vertex";
char* attributeNameNormal = "inNormal";
char* attributeNameColor = "inColor";
char* attributeNameTranslation = "inTranslation";
char* attributeNameRotation = "inRotation";
char* uniformNameScale = "inScale";

#endif