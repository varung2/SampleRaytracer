#ifndef CAMERA_H
#define CAMERA_H
#define M_PI 3.14159265358979323846264338327950288

#include "ray.h"

class camera {
public:
	camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect) {
		vec3 u, v, w;
		float theta = vfov*M_PI/180.0;
		float half_height = tan(theta/2);
		float half_width = aspect*half_height;

		origin = lookfrom;
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		look = lookat;
		lower_left_corner = vec3(-half_width, -half_height, -1.0);
		lower_left_corner = origin - half_width*u - half_height*v - w;
		horizontal = 2*half_width*u;
		vertical = 2*half_height*v;
	}

	ray get_ray(float u, float v, bool p) {
		//the first one gives the perspective view
		if (!p) {
			return ray(origin, lower_left_corner + u*horizontal + v*vertical - origin); 
		}
		//this gives the orthographic view 
		else {
			return ray(vec3(horizontal.x()*u + lower_left_corner.x(), vertical.y()*v + lower_left_corner.y(), 0.0), look);
		}
	}

	vec3 origin;
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 look;
};

#endif