#ifndef CUBE_H
#define CUBE_H

#include "hitable.h"
#include "material.h"
#include "vec3/vec3.h"

class cube : public hitable {
public:
	cube() {}
	cube(const vec3 &vmin, const vec3 &vmax, material *m) {
		bounds[0] = vmin; bounds[1] = vmax; 
		mat_ptr = m;
	};

	virtual bool hit(const ray& r, float tmin, float tmax, hit_record &hit) const;

	//member variables
	vec3 bounds[2];
	material *mat_ptr;
};

bool cube::hit(const ray& r, float timemin, float timemax, hit_record &hit) const {
	float tmin, tmax, tymin, tymax, tzmin, tzmax; //parameters for x, y, z
	bool xface, yface, zface, facemax;
	bool exitx, exity, exitz;
	tmin = (bounds[r.sign[0]].x() - r.origin().x()) * r.invdir.x();
	tmax = (bounds[1-r.sign[0]].x() - r.origin().x()) * r.invdir.x();
	tymin = (bounds[r.sign[1]].y() - r.origin().y()) * r.invdir.y();
	tymax = (bounds[1-r.sign[1]].y() - r.origin().y()) * r.invdir.y();
	
	//initializing booleans
	xface = false; yface = false; zface = false; facemax = false;
	exitx = false; exity = false; exitz = false;

	//checking if the ray actually hits something in the x,y plane first
	if ((tmin > tymax) || (tymin > tmax)) {
		return false;
	} else {
		xface = true; //initially hitting the xface
		exitx = true; //initially exiting the xmax
		facemax = false;
		//this means that it is hitting the xmin bound first and that
		//the ymin bound is the next hit, so if tymin is greater than tmin,
		//Then it is hitting the bottom face.
		if (tymin > tmin) {
			tmin = tymin;
			yface = true;
		}

		//This means that when the ray is exiting the bounding box, it is hitting the ymax bound first.
		if (tymax < tmax) {
			tmax = tymax;
			exity = true;
		}

		//calculating tparamters for the Zplane
		tzmin = (bounds[r.sign[2]].z() - r.origin().z()) * r.invdir.z();
		tzmax = (bounds[1 - r.sign[2]].z() - r.origin().z()) * r.invdir.z();

		//Now we are checking the boundaries of the height instead of on one plane,
		if ((tmin > tzmax) || (tzmin > tmax)) {
			return false;
		} else {

			if (tzmin > tmin) {
				tmin = tzmin;
				zface = true;
			}

			if (tzmax < tmax) {
				tmax = tzmax;
				exitz = true;
			}

			//tmin is the first hit so using tmin as paramter t unless it is less than 0;
			float t = tmin;
			if (t < 0) {
				t = tmax;
				facemax = true;
				if (t < 0) return false;
			}

			//t is the final paramter given.
			if (t < timemax && t > timemin) {
				rec.t = t;
				rec.p = r.point_at_parameter(rec.t);

				//calculating the normals of the faces based on what it hits first using booleans
				//first if it is not hitting the maximum boundaries first, ie behind the cube
				if (facemax == false) {
					//if hitting xmin face
					if (xface == true && yface == false && zface == false) {

					} 
					else if (xface == true && yface == true && zface == false) {

					} 
					else if (xface == true && yface == true && zface == true) {
						
					}
				}

				//if it hits the maximum boundaries first 
				else {

				}

				rec.mat_ptr = mat_ptr;
				return true;
			}
		}
	}
	return false;
}