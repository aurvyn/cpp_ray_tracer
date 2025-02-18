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
		float v = b + (t - b)*(y+0.5)/ny;
		
		Vector3 s = u*camera.getU() + v*camera.getV() - d*camera.getW();
		Ray ray;
		ray.setDirection(s);
		ray.setOrigin(camera.getPos());
		
		return ray;
	}

	RayPacket getRayPacket(size_t const & x, size_t const & y) const
	{
		int nx = w;
		int ny = h;
		float l, r, b, t;
		l = -(w/2.0f);
		r =  w/2.0f;
		b =  h/2.0f;
		t = -(h/2.0f);
		
		float d = h/2.0f * sin(camera.getFov());
		Vector3 dirs[N*N];
		for (int j = 0; j < N; j++){
			for (int i = 0; i < N; i++){
				float u = l + (r - l)*((x*N)+i+0.5)/nx;
				float v = b + (t - b)*((y*N)+j+0.5)/ny;
				
				Vector3 s = u*camera.getU() + v*camera.getV() - d*camera.getW();
				dirs[i+(j*N)] = s;
			}
		}
		RayPacket rays;
		rays.setDirection(dirs);
		rays.setOrigin(camera.getPos());
		
		return rays;
	}
	
private:
	Camera camera;
	size_t w, h;
};

#endif

