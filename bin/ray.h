#ifndef RAY_H
#define RAY_H
#include "vec3.h"

class ray {
public:
	ray() {}
	ray(const vec3& a, const vec3& b, float ti = 0.0) {A = a; B = b; invdir = 1.0 / b;
		sign[0] = (invdir.x() < 0);
		sign[1] = (invdir.y() < 0);
		sign[2] = (invdir.z() < 0);
		_time = ti;
	}

	vec3 origin() const {return A;}
	vec3 direction() const {return B;}
	vec3 point_at_parameter(float t) const {return A + t*B;}
	float time() const { return _time; }
	//vectors for the ray, A is the origin, B is the direction
	vec3 A;
	vec3 B;
	float _time;

	vec3 invdir;
	int sign[3];

};

#endif