#ifndef __RAYTRACER_H
#define __RAYTRACER_H

#include "GenVector.h"
#include "Buffer.h"
#include "Camera.h"
#include "Scene.h"
#include "RayGenerator.h"
#include "PrimitiveArray.h"
#include "Shader.h"
#include <omp.h>

class RayTracer
{
public:
	void trace(Scene & scene, size_t resX, size_t resY, unsigned char * outputImage)
	{
		Buffer<Color> imageBuffer = Buffer<Color>(resX, resY);
		Buffer<Vector3> floatBuffer = Buffer<Vector3>(resX, resY);
		bool packets = true;
		RayGenerator generator = RayGenerator(scene.getCamera(), resX, resY);
		if (packets){
			// #pragma omp parallel for
			for(int y=0; y<resY / M; y++)
			{
				for(int x=0; x<resX / N; x++)
				{
					RayPacket rays = generator.getRayPacket(x, y);
					std::array<bool, N*M> hitSomething;
					Hitpoint hits[N*M];
					hitSomething = scene.getRootPrimitive()->packetIntersect(rays, hits);
					for (int j = 0; j < M; j++){
						for (int i = 0; i < N; i++){
							int index = i + j * N;
							if(hitSomething[index]) {
								Vector3 dirs;
								for (int k = 0; k < 3; k++){
									 dirs[k] = rays.getDirections()[k][index];
								}
								Vector3 floatColor = Shader::shade(Ray(dirs, rays.getOrigin()), hits[index], scene);
								floatBuffer.at(x*N+i,y*M+j) = floatColor;
								//floatBuffer.at(x,y) = Vector3(0.0f);
							}
							else
								floatBuffer.at(x*N+i,y*M+j) = Vector3(0,0,0);
						}
					}
				}
			}
		} else {
			#pragma omp parallel for
			for(int y=0; y<resY; y++)
			{
				for(int x=0; x<resX; x++)
				{
					Ray ray = generator.getRay(x, y);

					//Day2
					//Ray r = generator.getRay(x, y);
					//Vector3 d = r.getDirection()*255.0f;
					//Color c = Color( abs(d[0]), abs(d[1]), abs(d[2]) );
					//floatBuffer.at(x,y) = d;
					//continue;
					
					// Vector3 rc = ray.getDirection();
					// rc = Vector3(fabs(rc[0]), fabs(rc[1]), fabs(rc[2]));

					bool hitSomething = false;
					Hitpoint hit;
					hitSomething = scene.getRootPrimitive()->intersect(ray, hit);
					if(hitSomething) {
						Vector3 floatColor = Shader::shade(ray, hit, scene);
						floatBuffer.at(x,y) = floatColor;
						//floatBuffer.at(x,y) = Vector3(0.0f);
					}
					else
						floatBuffer.at(x,y) = Vector3(0.0f);

					
				}
			}
		}

		toneMap(floatBuffer, imageBuffer);

		for(int y=0; y<resY; y++)
		{
			for(int x=0; x<resX; x++)
			{
				Vector3 v = floatBuffer.at(x,y) * 255.0f;
				Color c = Color(v[0], v[1], v[2]);
				imageBuffer.at(x,y) = c;
			}
		}

		unsigned char * renderBuffer = (unsigned char*)&imageBuffer.at(0,0);
		for(int i=0; i<resX*resY*3; i++)
		{
			outputImage[i] = renderBuffer[i];
		}
	}

private:

	void toneMap(Buffer<Vector3> & floatBuffer, Buffer<Color> & imageBuffer) const
	{
		float maxValue = 0.0f;
		size_t resX = imageBuffer.getWidth();
		size_t resY = imageBuffer.getHeight();

		for(int y=0; y<resY; y++)
		{
			for(int x=0; x<resX; x++)
			{
				Vector3 floatColor = floatBuffer.at(x,y);
				float maxComp = floatColor.c[floatColor.maxComponent()];
				if(maxComp > maxValue)
					maxValue = maxComp;
			}
		}

		if(maxValue <= 0.0f)
			maxValue = 1.0f;
		
		float toneMappingScale = 1.0f / maxValue;

		for(int y=0; y<resY; y++)
		{
			for(int x=0; x<resX; x++)
			{
				floatBuffer.at(x,y) = floatBuffer.at(x,y)*toneMappingScale;
			}
		}
	}
};

#endif

