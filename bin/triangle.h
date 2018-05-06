#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hitable.h"
#include "material.h"
#include <math.h>

float smol(const vec3& A, const vec3& B, const vec3& C, int idx) {
	float temp = A[idx];
	if (B[idx] < temp) {
		temp = B[idx];
	}
	if (C[idx] < temp) {
		temp = C[idx];
	}
	return temp;
}

float larg(const vec3& A, const vec3& B, const vec3& C, int idx) {
	float temp = A[idx];
	if (B[idx] > temp) {
		temp = B[idx];
	}
	if (C[idx] > temp) {
		temp = C[idx];
	}
	return temp;
}

class triangle : public hitable {
public:
	triangle() {}
	triangle(const vec3& A, const vec3& B, const vec3& C, material *m) {
		corner = A;
		BA = B - A;
		CB = C - B;
		AC = A - C; 
		mat_ptr = m;
		N = cross(BA, CB);
		a = A; b = B; c = C;
	}
	virtual bool hit(const ray& r, float tmin, float tmax, hit_record &hit) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;
	//member variables
	vec3 a, b, c; //points
	vec3 AC, BA, CB, N, corner; //vectors
	material *mat_ptr;
};

bool triangle::hit(const ray& r, float tmin, float tmax, hit_record &rec) const {
	//First calculate the point of intersection on the plane
	float x = dot(N, r.direction());
	if (fabs(x) < 0.001) return false;

	float t = (dot(corner - r.origin(), N)/x);

	if (t < 0) return false;
	vec3 p = r.point_at_parameter(t);

	vec3 AP = p - a;
	vec3 CP = p - c;
	vec3 BP = p - b;

	//topleft
	vec3 t0 = cross(BA, AP);
	if (dot(N, t0) < 0) return false;
	//topright
	vec3 t1 = cross(CB, BP);
	if (dot(N, t1) < 0) return false;
	//bottom
	vec3 t2 = cross(AC, CP);
	if (dot(N, t2) < 0) return false;


	if (t < tmax && t > tmin) {
		rec.t = t;
		rec.p = p;
		rec.normal = unit_vector(N);
		rec.mat_ptr = mat_ptr;
		return true;
	}

	return false;
}

bool triangle::bounding_box(float t0, float t1, aabb& box) const {
	vec3 small(smol(a, b, c, 0), smol(a, b, c, 1), smol(a, b, c, 2)); //gives the point with the smallest values of the triangle
	vec3 large(larg(a, b, c, 0), larg(a, b, c, 1), larg(a, b, c, 2)); //gives the point with the largest values of the triangle

	box = aabb(small, large);
	return true;
}


#endif