#include <GL/glew.h>

#ifndef SHADERS
#define SHADERS

GLuint LoadShaderProgram(const char *vert_path = "vertex.glsl", const char *frag_path = "fragment.glsl");

char* attribute_color_name = "inColor";
char* attribute_translation_name = "inTranslation";

#endif