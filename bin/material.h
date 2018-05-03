#ifndef MATERIAL_H
#define MATERIAL_H

#include "ray.h"
#include "hitable.h"
#include <math.h>

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
	vec3 uv = unit_vector(v);
	float dt = dot(uv, n);
	float discrim = 1.0 - ni_over_nt*ni_over_nt*(1-dt*dt);
	if (discrim > 0) {
		refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discrim);
		return true;
	} else 
		return false;
}

float drand48() {return (rand()/(RAND_MAX + 1.0));}

vec3 random_in_unit_sphere() {
	vec3 p;
	do { p = 2.0*vec3(drand48(), drand48(), drand48()) - vec3(1, 1, 1);
	} while (p.squared_length() >= 1.0);
	return p;
}
vec3 reflect(const vec3& v, const vec3& n) { 
	return v - 2*dot(v,n)*n;
}

class material {
public:
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
};

class lambertian : public material {
public:
	lambertian(const vec3& a) : albedo(a) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = ray(rec.p, target-rec.p);
		attenuation = albedo;
		return true;
	}

	vec3 albedo;
};

class metal : public material {
public:
	metal(const vec3& a, float f) : albedo(a) { if (f<1) fuz = f; else fuz = 1;}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered ) const {
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected + fuz*random_in_unit_sphere());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}
	vec3 albedo;
	float fuz;
};

class dielectric : public material {
public:
	dielectric(float ri) : ref_idx(ri) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 outward_normal;
		vec3 reflected = reflect(r_in.direction(), rec.normal);
		float ni_over_nt;
		attenuation = vec3(1.0, 1.0, 0.0);
		vec3 refracted;

		if (dot(r_in.direction(), rec.normal) > 0) {
			outward_normal = -rec.normal;
			ni_over_nt = ref_idx;
		}
		else {
			outward_normal = rec.normal;
			ni_over_nt = 1.0 / ref_idx;
		}

		if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
			scattered = ray(rec.p, refracted);
		} else {
			scattered = ray(rec.p, reflected);
			return false;
		}
		return true;
	}

	float ref_idx;
};

#endif