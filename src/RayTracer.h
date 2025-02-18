#ifndef __RAYTRACER_H
#define __RAYTRACER_H

#include <random>

#include "GenVector.h"
#include "Buffer.h"
#include "Camera.h"
#include "Scene.h"
#include "RayGenerator.h"
#include "PrimitiveArray.h"
#include "Shader.h"
#include <omp.h>

#include "PathTracer.h"

#define MAX_TRACE_DEPTH 2
#define TWO_PI 6.28318530718f

class RayTracer
{
public:
	void trace(Scene &scene, size_t resX, size_t resY, size_t rpp, unsigned int seed, unsigned char *outputImage)
	{
		Buffer<Color> imageBuffer(resX, resY);
		Buffer<Vector3> floatBuffer(resX, resY);
		Buffer<std::vector<FullPath>*> pathsBuffer(resX, resY);

		PathTracer pathTracer;
		RayGenerator generator(scene.getCamera(), resX, resY);

		#pragma omp parallel for
		for (int y = 0; y < resY; y++)
		{
			unsigned localseed = seed ^ ((y + resY) * 0x5E7B);
			
			for (int x = 0; x < resX; x++)
			{
				std::vector<FullPath> *pixelPaths = new std::vector<FullPath>();
				Ray ray = generator.getRay(x, y);
				for (int i = 0; i < (int)rpp; i++) {
					std::vector<FullPath> pixels = pathTracer.trace(ray, scene, MAX_TRACE_DEPTH, -1, &localseed);
					pixelPaths->insert(pixelPaths->end(), pixels.begin(), pixels.end());
				}
				pathsBuffer.at(x, y) = pixelPaths;
			}
		}
		
		std::vector<FullPath> globalLightPaths;
		std::vector<Light*> lights = scene.getLights();
        for (Light *light : lights)
        {
			globalLightPaths.push_back(FullPath(light->getPosition(), light->getMaterialId(), std::vector<HitDetails>(), std::vector<float>()));
            for (int j = 0; j < ((int)rpp) * resX * resY; j++)
            {
                Ray lightRay = pathTracer.sampleLightRay(*light, &seed);
                std::vector<FullPath> lightPaths = pathTracer.trace(lightRay, scene, MAX_TRACE_DEPTH, light->getMaterialId(), &seed);
                globalLightPaths.insert(globalLightPaths.end(), lightPaths.begin(), lightPaths.end());
            }
        }
		
		#pragma omp parallel for
        for (int y = 0; y < resY; y++)
        {
            for (int x = 0; x < resX; x++)
            {
                std::vector<FullPath> *pixelPaths = pathsBuffer.at(x, y);
                Vector3 accumulatedColor(0, 0, 0);
                for (FullPath &camPath : *pixelPaths)
                {
                    for (int i = 0; i < globalLightPaths.size(); i++)
                    {
						FullPath &lightPath = globalLightPaths.at(i);
						FullPath combinedPaths = pathTracer.combine(camPath, lightPath, scene);
						accumulatedColor += pathTracer.getColor(combinedPaths, scene, Vector3(198, 252, 255) * (1 / 255.0f / 4.0f));
                    }
                }
                floatBuffer.at(x, y) = accumulatedColor;
            }
        }
		
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
			for (int x = 0; x < resX; x++)
				floatBuffer.at(x, y) *= toneMappingScale;
	}
};

#endif
