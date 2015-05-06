#ifndef OBJ
#define OBJ

#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include "vertex.h"
#include <vector>

using namespace std;

struct triangle {
	int i1;
	int i2;
	int i3;
};

// Assumes only triangles, and that file contains only vertex and face lines.
void read_obj_file(const char* filename, vector<vertex>* vertices, vector<triangle>* triangles)
{
	ifstream ifs;
	string first_word;
	vertex new_vertex;
	triangle new_triangle;

	// Open file.
	ifs.open(filename);

	// Process file, one line at a time.
	do {
		// Start a line, and process it if the file's not empty.
		first_word.clear();  // (Then if line is empty, first_word won't persist from last line.)
		ifs >> first_word;
		if (ifs.eof() == false) {
			if (first_word == "v") {
				// Read a vertex line.
				ifs >> new_vertex.x >> new_vertex.y >> new_vertex.z;
				vertices->push_back(new_vertex);
			}
			else if (first_word == "f") {
				// Read a face line.
				ifs >> new_triangle.i1 >> new_triangle.i2 >> new_triangle.i3;
				// Decrement indices from [1,n] in obj file to [0,n-1] in vertex vector.
				--new_triangle.i1;
				--new_triangle.i2;
				--new_triangle.i3;
				triangles->push_back(new_triangle);
			}
			// Get rid of anything left on this line (including the newline).
			ifs.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	} while (ifs.eof() == false);

	// Close file.
	ifs.close();
}

#endif