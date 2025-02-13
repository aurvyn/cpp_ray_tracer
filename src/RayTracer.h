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

#include "PathTracer.h"

using namespace std;

#define MAX_TRACE_DEPTH (20)

class RayTracer
{
public:
	void trace(Scene & scene, size_t resX, size_t resY, size_t rpp, unsigned char * outputImage)
	{
		Buffer<Color> imageBuffer = Buffer<Color>(resX, resY);
		Buffer<Vector3> floatBuffer = Buffer<Vector3>(resX, resY);
		Buffer<vector<vector<Path>>*> pathsBuffer = Buffer<vector<vector<Path>>*>(resX, resY);
		
		//TODO use this to trace each ray and accumulate the paths
		PathTracer pathTracer;
	
		RayGenerator generator = RayGenerator(scene.getCamera(), resX, resY);
		
		#pragma omp parallel for
		for(int y=0; y<resY; y++)
		{
			for(int x=0; x<resX; x++)
			{
				vector<vector<Path>>* pixelPaths = new vector<vector<Path>>;
				Ray ray = generator.getRay(x, y);
				for(int i=0; i<rpp; i++) pixelPaths -> push_back(pathTracer.trace(ray, scene, MAX_TRACE_DEPTH));
				pathsBuffer.at(x,y) = pixelPaths;
			}
		}
		
		//TODO for each light, trace a bunch of points on it and accumulate those too
		
		//TODO stitch the camera and light paths together n-to-n, using PathTracer::combine.
		
		//TODO compute the color for each path and accumulate it into the output buffer

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

