#include <GL/glew.h>
#include <GL/freeglut.h>

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 033:
	case 'q':
		exit(1);
		break;

	}
}