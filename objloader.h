#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "bin/ray.h"
#include "bin/hitablelist.h"
#include "float.h"
#include "bin/sphere.h"
#include "bin/camera.h"
#include "bin/plane.h"
#include "bin/triangle.h"
#include "bin/bvh_node_old.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <queue>

using namespace std;

hitable* objloader(char* filename) {
	ifstream object;
	object.open(filename);

	if (!object.is_open()) {
		cout << "Failed to open file, returning NULL.\n";
		return NULL;
	}

	vector<vec3> vert;
	vert.push_back(vec3(0,0,0)); //pushing dummy vertex
	while (1) {

		if (c == "v")
	}

}






#endif