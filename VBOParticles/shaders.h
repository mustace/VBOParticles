#include <GL/glew.h>

#ifndef SHADERS
#define SHADERS

GLuint LoadShaderProgram(const char *vert_path = "vertex.glsl", const char *frag_path = "fragment.glsl");

char* attribute_color_name = "inColor";
char* attribute_translation_name = "inTranslation";
char* attribute_position_name = "inPosition";
char* attribute_normal_name = "inNormal";

#endif