#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <iomanip>

#include "bin/ray.h"
#include "bin/hitablelist.h"
#include "float.h"
#include "bin/material.h"
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
#include <string>
#include <unordered_map>

using namespace std;

material* Gen_Rand_Lamb() {
	return new lambertian(new constant_texture(vec3(drand48(), drand48(), drand48())));
}

material* Gen_Rand_Metal() {
	return new metal(vec3(drand48(), drand48(), drand48()), drand48());
}

material* Gen_Rand_White_Diffuse(float mult) {
	float rand = drand48()*mult;
	return new diffuse_light(new constant_texture(vec3(rand, rand, rand)));
}

material* Gen_Rand_Color_Diffuse() {
	return new diffuse_light(new constant_texture(vec3(drand48(), drand48(), drand48())));
}

hitable* objloader(string filename, int mult) {
	ifstream object;
	object.open(filename);

	if (!object.is_open()) {
		cout << "Failed to open file, returning NULL.\n";
		return NULL;
	}
	//for object creation
	hitable **boxlist = new hitable*[10000000]; //bvh with max 100K elements
	int j = 0;

	//for file parsing
	vector<vec3> vert;
	vert.push_back(vec3(0,0,0)); //pushing dummy vertex

	char *garbage = new char[50];
	char *buffer = new char[20]; //character buffer for 
	char delim = ' '; //assinging space as the delimiting character
	char nl = '\n'; //newline character
	std::unordered_map<string, material*> mat_table; //material of tables
	material* mat_curr;


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
			//use the stored index to getline the verticies
			boxlist[j++] = new triangle(vert[idx[0]], vert[idx[1]], vert[idx[2]], mat_curr);
		} 

		//detect object (currently no use)
		else if (strcmp(buffer, "o") == 0) {
			object.getline(garbage, 50);
		} 

		//detect object material (and add if necessary)
		else if (strcmp(buffer, "usemtl") == 0) {
			object.getline(buffer, 20); //get the material name (key)
			string mattype(buffer); //convert to string
			auto mat_iter = mat_table.find(mattype);
			if (mat_iter == mat_table.end()) { //if the material doesn't exist, add
				material* newmat = Gen_Rand_Metal();
				std::pair<string, material*> ins(mattype, newmat);
				mat_table.insert(ins);
			}
			mat_curr = mat_table[mattype];
		}

		//if the line is some random line either a comment or a mistake, ignore and move to the next line
		else {
			object.getline(garbage, 50);
		}

		object.getline(buffer, 20, delim);//get the next line character
		std::cout << "\rNumber of triangles: " << std::setw(6) << std::setfill('0') << vert.size() << ". Size of bvh: " << std::setw(6) << std::setfill('0') << j << std::flush;
	}

	std::cout << "\n";
	return new bvh_node(boxlist, j, 0.0, 1.0);
}

#endif