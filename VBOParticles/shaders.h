#ifndef SHADERS
#define SHADERS

#include <GL/glew.h>

GLuint LoadShaderProgram(const char *vert_path = "vertex.glsl", const char *frag_path = "fragment.glsl");

char* attribute_name_color = "inColor";
char* attribute_name_translation = "inTranslation";
char* attribute_name_rotation = "inRotation";
char* attribute_name_normal = "inNormal";
char* attribute_name_vertex = "inVertex";

#endif