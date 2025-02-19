#ifndef __RAYGEN
#define __RAYGEN

#define _USE_MATH_DEFINES
#include <math.h>
#include "Ray.h"
#include "Camera.h"

Vector3 project(Vector3 a, Vector3 b) {
	// project a onto b
	Vector3 b_hat = Vector3(b).normalize();
	return a.dot(b_hat)*b_hat;
}

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
		
		size_t test_x, test_y;
		getXY(test_x, test_y, ray);
		printf("test x: %zu, x: %zu \n", test_x, x);
		// printf("test y: %zu, y: %zu \n", test_y, y);
		assert(x == test_x);
		assert(y == test_y);

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
		// because the direction has been normalized we need to un normalize it
		float scale = d/project(ray.getDirection(), camera.getW()).length();
		Vector3 dir = ray.getDirection()*scale;
		printf("cdir: %f %f %f\n", dir[0], dir[1], dir[2]);

		Vector3 projDirOntoU = project(dir, camera.getU());
		float u = projDirOntoU.length()/camera.getU().length()*(camera.getU().dot(projDirOntoU) > 0 ? 1 : - 1);
		printf("calcu: %f\n", u);
		float x_res = roundf(((u - l)*nx)/(r - l) - 0.5);
		printf("x_res: %f\n", x_res);
		x = (size_t)x_res;

		Vector3 projDirOntoV = project(dir, camera.getV());
		float v = projDirOntoV.length()/camera.getV().length()*(camera.getV().dot(projDirOntoV) > 0 ? 1 : - 1);
		printf("calcu: %f\n", v);
		float y_res = roundf(((v - b)*nx)/(t - b) - 0.5);
		y = (size_t)y_res;
	}
	
private:
	Camera camera;
	size_t w, h;
};

#endif

