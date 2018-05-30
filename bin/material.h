#ifndef MATERIAL_H
#define MATERIAL_H

#include "ray.h"
#include "hitable.h"
#include <math.h>

float schlick(float cosine, float ref_idx) {
	float r0 = (1-ref_idx) / (1+ref_idx);
	r0 = r0*r0;
	return r0 + (1-r0)*pow((1-cosine), 5);
}


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

//perlin class---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------

	class perlin {
	public:
		float noise(const vec3& p) const {
			// float u = p.x() - floor(p.x());
			// float v = p.y() - floor(p.y());
			// float w = p.z() - floor(p.z());
			int i = int(4*p.x()) & 255;
			int j = int(4*p.y()) & 255;
			int k = int(4*p.z()) & 255;
			return ranfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
		}
		static float *ranfloat;
		static int *perm_x;
		static int *perm_y;
		static int *perm_z;
	};

	static float* perlin_generate() {
		float *p = new float[256];
		for (int i = 0; i < 256; ++i)
			p[i] = drand48();
		return p;
	}

	void permute(int *p, int n) {
		for (int i = n-1; i > 0; i--) {
			int target = int(drand48()*(i+1));
			int tmp = p[i];
			p[i] = p[target];
			p[target] = tmp;
		}
		return;
	}

	static int* perlin_generate_perm() {
		int *p  = new int[256];
		for (int i = 0; i < 256; i++) 
			p[i] = i;
		permute(p, 256);
		return p;
	}
	 
	float *perlin::ranfloat = perlin_generate();
	int *perlin::perm_x = perlin_generate_perm();
	int *perlin::perm_y = perlin_generate_perm();
	int *perlin::perm_z = perlin_generate_perm();

//END perlin class-----------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------


//texture class--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
class texture {
	public:
		virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class constant_texture : public texture {
public:
	constant_texture() {}
	constant_texture(vec3 c) : color(c) {}
	virtual vec3 value(float u, float v, const vec3& p) const {return color;}
	vec3 color;
};

class checker_texture : public texture {
public:
	checker_texture() {}
	checker_texture(texture *t0, texture *t1) : even(t0), odd(t1) { }
	virtual vec3 value(float u, float v, const vec3& p) const {
		float sines = sin(10*p.x())*sin(10*p.y())*sin(10*p.z());
		if (sines < 0)
			return odd->value(u, v, p);
		else
			return even->value(u, v, p);
	}

	texture *even;
	texture *odd;
};

class noise_texture : public texture {
public:
	noise_texture() {}
	virtual vec3 value(float u, float v, const vec3& p) const {
		return vec3(1, 1, 1)*noise.noise(p);
	}
	perlin noise;
};

//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------


class material {
public:
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
	virtual vec3 emitted(float u, float v, const vec3& p) const { return vec3(0,0,0); }
};

class lambertian : public material {
public:
	lambertian() {}
	lambertian(texture *a) : albedo(a) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = ray(rec.p, target-rec.p);
		attenuation = albedo->value(0, 0, rec.p);
		return true;
	}


	texture* albedo;
};

class metal : public material {
public:
	metal() {}
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
	dielectric () {}
	dielectric(float ri) : ref_idx(ri) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 outward_normal;
		vec3 reflected = reflect(r_in.direction(), rec.normal);
		float ni_over_nt;
		attenuation = vec3(1.0, 1.0, 1.0);
		vec3 refracted;

		float reflect_prob;
		float cosine;
		if (dot(r_in.direction(), rec.normal) > 0) {
			outward_normal = -rec.normal;
			ni_over_nt = ref_idx;
			cosine = ref_idx*dot(r_in.direction(), rec.normal) / r_in.direction().length();
		}
		else {
			outward_normal = rec.normal;
			ni_over_nt = 1.0 / ref_idx;
			cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
		}

		if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
			reflect_prob = schlick(cosine, ref_idx);
		} else {
			scattered = ray(rec.p, reflected);
			reflect_prob = 1.0;
		}

		if (drand48() < reflect_prob) {
			scattered = ray(rec.p, reflected);
		} else {
			scattered = ray(rec.p, refracted);
		}
		return true;
	}

	float ref_idx;
};

class diffuse_light : public material {
	public :
		diffuse_light() {}
		diffuse_light(texture *a) : emit(a) {}
		virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {return false;}
		virtual vec3 emitted(float u, float v, const vec3& p) const {
			return emit->value(u, v, p); 
		}
		texture *emit;
};


#endif