#ifndef SHADERS
#define SHADERS

#include <GL/glew.h>

GLuint LoadShaderProgram(const char *vert_path = "vertex.glsl", const char *frag_path = "fragment.glsl");

char* uniform_name_campos = "camPos";
char* attributeNameVertex = "inVertex";
char* attributeNameNormal = "inNormal";
char* attributeNameColor = "inColor";
char* attributeNameTranslation = "inTranslation";
char* attributeNameRotation = "inRotation";
char* uniformNameScale = "inScale";
char* uniformNameReflecRate = "reflecRate";

#endif