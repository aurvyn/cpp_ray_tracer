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

#define MAX_TRACE_DEPTH (10)
#define TWO_PI 6.28318530718f

class RayTracer
{
public:
	void trace(Scene &scene, size_t resX, size_t resY, size_t rpp, unsigned char *outputImage)
	{
		Buffer<Color> imageBuffer = Buffer<Color>(resX, resY);
		Buffer<Vector3> floatBuffer = Buffer<Vector3>(resX, resY);
		Buffer<vector<FullPath> *> pathsBuffer = Buffer<vector<FullPath> *>(resX, resY);

		// TODO use this to trace each ray and accumulate the paths
		PathTracer pathTracer;

		RayGenerator generator = RayGenerator(scene.getCamera(), resX, resY);

		#pragma omp parallel for
		for (int y = 0; y < resY; y++)
		{
			for (int x = 0; x < resX; x++)
			{
				vector<FullPath> *pixelPaths = new vector<FullPath>;
				Ray ray = generator.getRay(x, y);
				for (int i = 0; i < (int)rpp; i++)
				{
					FullPath camPath = pathTracer.trace(ray, scene, MAX_TRACE_DEPTH, -1);
					pixelPaths->push_back(camPath);
				}
				pathsBuffer.at(x, y) = pixelPaths;
			}
		}

		// TODO for each light, trace a bunch of points on it and accumulate those too // added function below
		vector<FullPath> globalLightPaths;
        vector<Light*> lights = scene.getLights();
        for (int i = 0; i < lights.size(); i++)
        {
            for (int j = 0; j < (int)rpp; j++)
            {
				Light* light = lights.at(i);
                Ray lightRay = sampleLightRay(*light);
                FullPath lightPath = pathTracer.trace(lightRay, scene, MAX_TRACE_DEPTH, light->getMaterialId());
                globalLightPaths.push_back(lightPath);
            }
        }

		#pragma omp parallel for
        for (int y = 0; y < resY; y++)
        {
            for (int x = 0; x < resX; x++)
            {
                vector<FullPath>* pixelPaths = pathsBuffer.at(x, y);
                Vector3 accumulatedColor(0, 0, 0);
                for (auto &camPath : *pixelPaths)
                {
                    for (int i = 0; i < globalLightPaths.size(); i++)
                    {
						FullPath lightPath = globalLightPaths.at(i);
						FullPath combinedPaths = pathTracer.combine(camPath, lightPath, scene);
						accumulatedColor += pathTracer.getColor(combinedPaths, scene);
                    }
                }
                floatBuffer.at(x, y) = accumulatedColor;
            }
        }


		//clean the buffer
		for (int y = 0; y < resY; y++)
        {
            for (int x = 0; x < resX; x++)
            {
                delete pathsBuffer.at(x, y);
            }
        }

		//added

		// TODO compute the color for each path and accumulate it into the output buffer // done with above stitch

		toneMap(floatBuffer, imageBuffer);

		for (int y = 0; y < resY; y++)
		{
			for (int x = 0; x < resX; x++)
			{
				Vector3 v = floatBuffer.at(x, y) * 255.0f;
				Color c = Color(v[0], v[1], v[2]);
				imageBuffer.at(x, y) = c;
			}
		}

		unsigned char *renderBuffer = (unsigned char *)&imageBuffer.at(0, 0);
		for (int i = 0; i < resX * resY * 3; i++)
		{
			outputImage[i] = renderBuffer[i];
		}
	}

private:
	void toneMap(Buffer<Vector3> &floatBuffer, Buffer<Color> &imageBuffer) const
	{
		float maxValue = 0.0f;
		size_t resX = imageBuffer.getWidth();
		size_t resY = imageBuffer.getHeight();

		for (int y = 0; y < resY; y++)
		{
			for (int x = 0; x < resX; x++)
			{
				Vector3 floatColor = floatBuffer.at(x, y);
				float maxComp = floatColor.c[floatColor.maxComponent()];
				if (maxComp > maxValue)
					maxValue = maxComp;
			}
		}

		if (maxValue <= 0.0f)
			maxValue = 1.0f;

		float toneMappingScale = 1.0f / maxValue;

		for (int y = 0; y < resY; y++)
		{
			for (int x = 0; x < resX; x++)
			{
				floatBuffer.at(x, y) = floatBuffer.at(x, y) * toneMappingScale;
			}
		}
	}

    Ray sampleLightRay(const Light &light)
    {
        float u = Shader::RandomFloat(0.0f, 1.0f);
        float v = Shader::RandomFloat(0.0f, 1.0f);
        float theta = TWO_PI * u;
        float phi = acos(2.0f * v - 1.0f);
        float x = sin(phi) * cos(theta);
        float y = sin(phi) * sin(theta);
        float z = cos(phi);
        Vector3 dir(x, y, z);
        dir.normalize();
        
        return Ray(dir, light.getPosition());
    }
	//added
};

#endif
