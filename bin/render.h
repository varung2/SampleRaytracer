#include "../png/HSLAPixel.h"

struct pack {
	vec3 rgb;
	HSLAPixel px;
	unsigned int j;
	unsigned int k;
};

struct rgbpacket {
	unsigned char r, g, b;
	unsigned int j;
	unsigned int k;
};


vec3 color(const ray& r, hitable *world, int depth) {

	//object color
	hit_record rec;
	if (world->hit(r, 0.001, DBL_MAX, rec)) {
		//This gave the color of the Sphere
		// return 0.5*vec3(rec.normal.x()+1, rec.normal.y()+1, rec.normal.z()+1);

		//This gave the sphere a shadow by bouncing random rays around
		//vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		//return 0.5*color( ray(rec.p, target-rec.p), world);
		
		//this is for the objects it hits
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			vec3 i = attenuation*color(scattered, world, depth+1);

			return i;

			//phong shading, doesn't work with materials well.
			// if (depth == 0) {
			// 	float ph = fabs(dot(rec.normal, -1*r.direction()));
			// 	return (i*ph);
			// } else {
			// 	return (i);
			// }
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
