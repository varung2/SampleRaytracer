#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <iomanip>


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
#include <cstdlib>
#include <string>
#include <vector>

using namespace std;

hitable* objloader(string filename, int mult, material* mat) {
	ifstream object;
	object.open(filename);

	if (!object.is_open()) {
		cout << "Failed to open file, returning NULL.\n";
		return NULL;
	}
	//for object creation
	hitable **boxlist = new hitable*[10000000]; //bvh with max 100K elements
	int j = 0;
	material *grey = mat;

	//for file parsing
	vector<vec3> vert;
	vert.push_back(vec3(0,0,0)); //pushing dummy vertex

	char *buffer = new char[20]; //character buffer for 
	char delim = ' '; //assinging space as the delimiting character
	char nl = '\n'; //newline character
	
	object.getline(buffer, 20, delim);
	while (!object.eof()) {	
		//it is a vertex
		if (strcmp(buffer, "v") == 0) {
			vector<float> point(3);
			for (int i = 0; i < 3; i++) {
				if (i == 2) object.getline(buffer, 20, nl);
				else object.getline(buffer, 20, delim);
				point[i] = atof(buffer);
			}

			vert.push_back(vec3(point[0]*mult, point[1]*mult, point[2]*mult));
		}

		//create a face, ie a triangle
		else if (strcmp(buffer, "f") == 0) {

			vector<int> idx(3); //vector to store index value
			//getlineting the vertex indicies
			for (int i = 0; i < 3; i++) {
				if (i == 2) object.getline(buffer, 20, nl);
				else object.getline(buffer, 20, delim);
				idx[i] = atoi(buffer);
			}
			// vec3 v1 = vert[idx[0]];
			// vec3 v2 = vert[idx[1]];
			// vec3 v3 = vert[idx[2]];
			//use the stored index to getline the verticies
			boxlist[j++] = new triangle(vert[idx[0]], vert[idx[1]], vert[idx[2]], grey);
		}

		object.getline(buffer, 20, delim);//get the next line character
		std::cout << "\rNumber of triangles: " << std::setw(6) << std::setfill('0') << vert.size() << ". Size of bvh: " << std::setw(6) << std::setfill('0') << j << std::flush;
	}

	std::cout << "\n";
	// elem = j;
	return new bvh_node(boxlist, j, 0.0, 1.0);
}






#endif