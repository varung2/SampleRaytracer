#ifndef CAMERA_H
#define CAMERA_H
#define M_PI 3.14159265358979323846264338327950288

#include "ray.h"

vec3 random_in_unit_disk() {
	vec3 p;
	do {
		p = 2.0*vec3(drand48(), drand48(), 0) - vec3(1, 1, 0);
	} while (dot(p,p) >= 1.0);
	return p;
}

class camera {
public:
	camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist) {

		lens_radius = aperture/2;

		float theta = vfov*M_PI/180.0;
		float half_height = tan(theta/2);
		float half_width = aspect*half_height;

		origin = lookfrom;
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		look = lookat;
		lower_left_corner = origin - half_width*u*focus_dist - half_height*v*focus_dist - w*focus_dist;
		topleft = origin - half_width*u*focus_dist + half_height*v*focus_dist;
		horizontal = 2*half_width*focus_dist*u;
		vertical = 2*half_height*v*focus_dist;
	}

	ray get_ray(float s, float t, bool p) {
		vec3 rd = lens_radius*random_in_unit_disk();
		vec3 offset = u*rd.x() +v*rd.y();
		//the first one gives the perspective view
		if (!p) {
			return ray(origin + offset, lower_left_corner + s*horizontal + t*vertical - origin - offset); 
		}
		//this gives the orthographic view 
		else {
			return ray(topleft + s*horizontal + t*vertical, look);
		}
	}

	vec3 u, v, w;
	vec3 topleft;
	vec3 origin;
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 look;
	float lens_radius;

};

#endif