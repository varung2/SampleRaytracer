#ifndef AABB_H
#define AABB_H

#include "ray.h"
#include "vec3.h"
#include <cstdlib>
#include <algorithm>

class aabb {
public:
	aabb() {}
	aabb(const vec3& a, const vec3& b) { _min = a; _max = b;}

	vec3 min() const {return _min;}
	vec3 max() const {return _max;}

	bool hit(const ray& r, float tmin, float tmax) const {
		for (int a = 0; a < 3; a++) {
			float t0 = (_min[a] - r.origin()[a]) * r.invdir[a];
			float t1 = (_max[a] - r.origin()[a]) * r.invdir[a];
			if (r.invdir[a] < 0.0f) {
				std::swap(t0, t1);
			}

			tmin = (t0 > tmin) ? t0 : tmin;
			tmax = (t1 < tmax) ? t1 : tmax;
			if (tmax <= tmin)
				return false;
		}
		return true;
	}
	//member variables
	vec3 _min;
	vec3 _max;
};

#endif