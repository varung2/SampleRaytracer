#ifndef PLANE_H
#define PLANE_H

#include "hitable.h"
#include "material.h"
#include <math.h>

class plane : public hitable {
public:
	plane() {}
	plane(const vec3& norm, const vec3& cen, material *m){
		N = norm;
		center = cen;
		mat_ptr = m;
	}
	virtual bool bounding_box(float t0, float t1, aabb& box) const;
	virtual bool hit(const ray& r, float tmin, float tmax, hit_record &hit) const;
	
	//member variables
	vec3 N, center;
	material *mat_ptr;
};

bool plane::hit(const ray& r, float tmin, float tmax, hit_record &rec) const {
	if (fabs(dot(N, r.direction())) < 1e-6) {
		return false;
	} else {
		float t = (dot((center - r.origin()), N)/ dot(r.direction(), N));
		if (t < 0) {
			return false;
		}
		if (t < tmax && t > tmin) {
			rec.t = t;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = N;
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}

bool plane::bounding_box(float t0, float t1, aabb& box) const {
	return false;
}

#endif