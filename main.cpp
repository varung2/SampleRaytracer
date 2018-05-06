#define ELEM 1
#define XDIM 300
#define YDIM 300
#define MAX 300*300

//TESTING
#include <type_traits>
#include <ctime>

//Resources for multithreading
#include <thread>
#include <future>

//PNG FILES
#include "png/RGB_HSL.h"
#include "png/PNG.h"
#include "png/HSLAPixel.h"
#include "png/lodepng/lodepng.h"

//STL
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <queue>

//Raytracing header files
#include "bin/ray.h"
#include "bin/hitablelist.h"
#include "float.h"
#include "bin/sphere.h"
#include "bin/camera.h"
#include "bin/plane.h"
#include "bin/triangle.h"
#include "bin/bvh_node_old.h"
// #include "bin/aabb.h"
// #include "bin/rotate_y.h"
#include "bin/translate.h"

//#include "bvh/bvh_node.h"
#include "objloader.h"

using namespace std;
using namespace cs225;

/*
*	Information in README.txt
*
*	CS 296 MP1
*	
*	Last Edited: 3/9/18
*	@uthor: Varun Govind
*		  
*/

struct pack {
	vec3 rgb;
	HSLAPixel px;
	unsigned int j;
	unsigned int k;
};

struct rgbpacket {
	unsigned char r, g, b;
	unsigned int idx;
};


vec3 color(const ray& r, hitable *world, int depth) {
	
	//object color
	hit_record rec;
	if (world->hit(r, 0.001, DBL_MAX, rec)) {
		
		//this is for the objects it hits
		ray scattered;
		vec3 attenuation;
		if (depth < 700 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return (attenuation*color(scattered, world, depth+1));
		}
		else {
			return vec3(0,0,0);
		}
	}

	//background color
	else {
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5*(unit_direction.y() + 1.0);
		return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
	}
}

//WRAPPER FUNCTION FOR COLOR
vec3 draw(int j, int k, vec3 dim, hitable *world, camera &cam, bool orth) {
	//SECTION TO PARELLELIZE
	int nx = (int)dim[0];
	int ny = (int)dim[1];
	int ns = (int)dim[2];
	vec3 col(0, 0, 0);
	//shoot ns rays per pixel.
	for (int s = 0; s < ns; s++) {
		float u = float(k + drand48()) / float(nx);
		float v = (float(j + drand48()) / float(ny));
		ray r = cam.get_ray(u, v, orth);
		col += color(r, world, 0);
	}
	col /= float(ns);
	col = vec3(sqrt(col[0]-0.05), sqrt(col[1]-0.05), sqrt(col[2])-0.05);
	return col;
}

//WRAPPER FUNCTION FOR DRAW
//returns an rgbpacket to draw, for compression
rgbpacket render(int j, int k, vec3 dim, hitable *world, camera &cam, bool orth) {
	vec3 rgbcol = draw(j, k, dim, world, cam, orth);
	rgbpacket RGB_COL;
	RGB_COL.r = (unsigned char)(255.99*rgbcol[0]);
	RGB_COL.g = (unsigned char)(255.99*rgbcol[1]);
	RGB_COL.b = (unsigned char)(255.99*rgbcol[2]);

	return RGB_COL;
}

HSLAPixel renderHSLA(int j, int k, vec3 dim, hitable *world, camera &cam, bool orth) {
	vec3 rgbcol = draw(j, k, dim, world, cam, orth);
	rgbaColor RGB_COL;
	RGB_COL.r = (unsigned char)(255.99*rgbcol[0]);
	RGB_COL.g = (unsigned char)(255.99*rgbcol[1]);
	RGB_COL.b = (unsigned char)(255.99*rgbcol[2]);
	hslaColor pix = rgb2hsl(RGB_COL);
	HSLAPixel PX(pix.h, pix.s, pix.l);
	return PX;
}

int main(int argc, char **argv) {
	//start timer
	clock_t begin = clock();

	PNG img;
	bool orth = false;
	if (argc >= 2) {
		if (strcmp(argv[1], "-orthographic") == 0 || strcmp(argv[1], "-o") == 0 || strcmp(argv[1], "-orth") == 0 || strcmp(argv[1], "-ortho") == 0) {
			orth = true;
		}
	}

	cout << "Placing objects in world...\n";
	clock_t objectcreate = clock();
	//image dimensions
	int nx = 700;
	int ny = 350;
	int ns = 300; //number of rays per pixel, for antialiasing
	img.resize((unsigned int)nx, (unsigned int)ny);

	vec3 lookfrom(-20, -7, -9);
	vec3 lookat(9,.5,-1);
	float dist_to_focus = (lookfrom-lookat).length() + 1.0;
	float aperture = 0.001;
	camera cam(lookfrom, lookat, vec3(1.61,1,0), 60, float(nx)/float(ny), aperture, dist_to_focus);
	
	hitable *list[ELEM];
	hitable *world = new hitable_list(list, ELEM); //Elem is the predefined number of elements above
	int l = 0;
	//loading object
	int elements;
	hitable **object = objloader("bunny.obj", elements, 18);
	list[l++] = new bvh_node(object, elements, 0.0, 1.0);



	//spheres
	// list[l++] = new sphere(vec3(-0.1,-0.5,-1), .2, new lambertian(vec3(0.8, 0.3, 0.3)));
	// list[l++] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
	// list[l++] = new sphere(vec3(1, 4.5, -1), 5, new metal(vec3(0.8, 0.6, 0.2), 0.0));
	// list[l++] = new sphere(vec3(-1, 0,-1), .5, new dielectric(1.5));

	//plane, first one is the normal, second is the center
	// list[l++] = new plane(vec3(0, 1, 0), vec3(0, -0.4, -1), new lambertian(vec3(0.8, 0.3, 0.3)));

	//triangle initialization, points A, B, C
	// list[l++] = new triangle(vec3(-1, -1, -1), vec3(1.6, 0.0, -0.8), vec3(1.6, 1.5, -1.0), new metal(vec3(0.8, 0.8, 0.3), 0.3));
	

	// material *blue = new lambertian(vec3(0.2, 0.8, 1));
	// int na = 10000;
	// hitable **boxlist2 = new hitable*[10000];
	// for (int j = 0; j < na; j++) {
	// 	boxlist2[j] = new sphere(vec3(8*drand48(), 2*drand48(), 8*drand48()), .1, blue);
	// }
	// list[l++] = new translate(new bvh_node(boxlist2, na, 0.0, 1.0), vec3(-3, 1, -3.5)); 


	// material *grey = new lambertian(vec3(0.5, 0.5, 0.5));
	// int ne = 5000;
	// hitable **box2 = new hitable*[5000];
	// for (int j = 0; j < ne; j++) {
	// 	box2[j] = new sphere(vec3(4*drand48(), 2*drand48(), 4*drand48()), .1, grey);
	// }
	// list[l++] = new translate(new bvh_node(box2, ne, 0.0, 1.0), vec3(-3, 3, -5.5)); 

	// material *white = new lambertian(vec3(1, 1, 1));
	// int ng = 5000;
	// hitable **greenbox = new hitable*[5000];
	// for (int j = 0; j < ng; j++) {
	// 	greenbox[j] = new sphere(vec3(4*drand48(), 2*drand48(), 4*drand48()), .1, white);
	// }
	// list[l++] = new translate(new bvh_node(greenbox, ng, 0.0, 1.0), vec3(1, 3, -1.5)); 

	//creating containers
	vec3 dim(nx, ny, ns);
	int max = nx*ny;

	clock_t objectcreateend = clock();
	cout << "Done. (time: " << (double)(objectcreateend - objectcreate) / CLOCKS_PER_SEC << "sec)\n";

	//Parallel/Multithreaded implimentation
	volatile std::atomic<int> count(0);
	int cores = std::thread::hardware_concurrency();
	cores -= 1;
	vector<future<std::vector<rgbpacket>>> future_vector;
	cout << "Starting Rendering with " << cores << " cores...\n";
	while (cores--) {
		future_vector.emplace_back(
			std::async([=, &world, &cam, &dim, &orth, &count] {
				vector<rgbpacket> stack;
				while (true) {
					int index = count++;
					if (index >= max) {
						return stack;
					} else {
						int k = (index % nx);
						int j = (index / nx);

						rgbpacket curpx = render(j, k, dim, world, cam, orth);
						curpx.idx = (unsigned int)index;
						stack.push_back(curpx);
					}
				}
			})
		);
	}
	std::vector<vector<rgbpacket>> fullstack;
	for (int i = 0; i < (int)future_vector.size(); i++) {
		fullstack.push_back(future_vector[i].get());
 	}

	clock_t imgpars = clock();
	cout << "Done. (time: " << (double)((imgpars - objectcreateend) / CLOCKS_PER_SEC) << "sec)\n";
	cout << "Parsing image...\n";


	//Parsing the image buffer
	for (unsigned int i = 0; i < fullstack.size(); i++) {
		vector<rgbpacket> &stack = fullstack[i];

		for (unsigned int k = 0; k < stack.size(); k++) {
			rgbpacket &rgb_comp = stack[k];

			rgbaColor conv;
			conv.r = rgb_comp.r;
			conv.g = rgb_comp.g;
			conv.b = rgb_comp.b;

			hslaColor pix = rgb2hsl(conv);
			HSLAPixel newpixel(pix.h, pix.s, pix.l);

			HSLAPixel &imgpx = img.getPixel((rgb_comp.idx%nx), (rgb_comp.idx/nx));
			imgpx = newpixel;
		}
	}

	img.writeToFile("render.png");

	clock_t end = clock();
	cout << "Done. (time: " << double(end - imgpars) / CLOCKS_PER_SEC << "sec)\n";
	cout << "Total Runtime " << double(end - begin) / CLOCKS_PER_SEC << " seconds\n";
	return 0;
}