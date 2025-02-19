#ifndef __RAYGEN
#define __RAYGEN

#define _USE_MATH_DEFINES
#include <math.h>
#include "Ray.h"
#include "Camera.h"

class RayGenerator
{
public:
	RayGenerator()
	{
		this->w = 0;
		this->h = 0;
	}
	
	RayGenerator(Camera const & camera, size_t const & width, size_t const & height)
	{
		this->camera = camera;
		this->w = width;
		this->h = height;
	}
	
	Ray getRay(size_t const & x, size_t const & y) const
	{
		int nx = w;
		int ny = h;
		float l, r, b, t;
		l = -(w/2.0f);
		r =  w/2.0f;
		b =  h/2.0f;
		t = -(h/2.0f);
		
		float d = h/2.0f * sin(camera.getFov());
		float u = l + (r - l)*(x+0.5)/nx;
		printf("u: %f\n", u);
		float v = b + (t - b)*(y+0.5)/ny;
		
		Vector3 s = u*camera.getU() + v*camera.getV() - d*camera.getW();
		Ray ray;
		ray.setDirection(s);
		ray.setOrigin(camera.getPos());
		
		// size_t test_x, test_y;
		// getXY(test_x, test_y, ray);
		// printf("test x: %zu, x: %zu \n", test_x, x);
		// // printf("test y: %zu, y: %zu \n", test_y, y);
		// assert(x == test_x);

		return ray;
	}

	void getXY(size_t & x, size_t & y, Ray ray) const {
		// inverse of get ray
		// took forever to get working
		int nx = w;
		int ny = h;
		float l, r, b, t;
		l = -(w/2.0f);
		r =  w/2.0f;
		b =  h/2.0f;
		t = -(h/2.0f);
		

		assert(ray.getOrigin() == camera.getPos());

		float d = h/2.0f * sin(camera.getFov());
		Vector3 dir = ray.getDirection() + d*camera.getW();

		Vector3 projDirOntoU = (dir.dot(camera.getU()))*camera.getU();
		float u = projDirOntoU.length();
		printf("calcu: %f\n", u);
		float x_res = ((u - l)*nx)/(r - l) - 125.5;
		printf("x_res: %f\n", x_res);
		x = (size_t)x_res;

		// Vector3 v = camera.getV();
		// v = v.normalize();
		// Vector3 projDirOntoV = (dir.dot(v))*v;
		// // printf("%f %f %f\n", projDirOntoV[0], projDirOntoV[1], projDirOntoV[2]);
		// float y_res = ((projDirOntoV.length()/v.length()))*d + h/2.0f; // should be correct;
		// printf("%f\n", y_res);
		// y = (size_t)y_res;
	}
	
private:
	Camera camera;
	size_t w, h;
};

#endif

