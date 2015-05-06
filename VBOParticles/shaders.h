#ifndef SHADERS
#define SHADERS

#include <GL/glew.h>

GLuint LoadShaderProgram(const char *vert_path = "vertex.glsl", const char *frag_path = "fragment.glsl");

char* attribute_color_name = "inColor";
char* attribute_translation_name = "inTranslation";
char* attribut_rotz_name = "inRotZ";

#endif