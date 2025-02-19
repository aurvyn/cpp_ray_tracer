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
#include "ToneMapper.h"
#include <omp.h>

#include "PathTracer.h"

#define MAX_TRACE_DEPTH 2
#define TWO_PI 6.28318530718f

class RayTracer
{
public:
	void trace(Scene &scene, size_t resX, size_t resY, size_t rpp, size_t rpl, unsigned int seed, unsigned char *outputImage)
	{
		Buffer<Color> imageBuffer(resX, resY);
		Buffer<Vector3> floatBuffer(resX, resY);
		Buffer<std::vector<FullPath>*> pathsBuffer(resX, resY);

		PathTracer pathTracer;
		RayGenerator generator(scene.getCamera(), resX, resY);

		Material material;
		// material.setKd(Vector3(198, 252, 255) / 255.0f);
		material.setKd(Vector3());
		scene.addMaterial(material);
		Hitpoint background(INFINITY, Vector3(), scene.getMaterials().size() - 1);

		#pragma omp parallel for
		for (int y = 0; y < resY; y++)
		{
			unsigned localseed = seed ^ ((y + resY) * 0x5E7B);
			
			for (int x = 0; x < resX; x++)
			{
				std::vector<FullPath> *pixelPaths = new std::vector<FullPath>();
				Ray ray = generator.getRay(x, y);
				pixelPaths->push_back(FullPath(ray.getOrigin(), -1, {}, {}));
				for (int i = 0; i < (int)rpp; i++) {
					std::vector<FullPath> pixels = pathTracer.trace(ray, scene, MAX_TRACE_DEPTH, -1, &localseed);
					pixelPaths->insert(pixelPaths->end(), pixels.begin(), pixels.end());
				}
				// printf("(%d, %d): %d\n", x, y, pixelPaths->size());
				pathsBuffer.at(x, y) = pixelPaths;
			}
		}
		
		std::vector<FullPath> globalLightPaths;
		std::vector<Light*> lights = scene.getLights();
        for (Light *light : lights)
        {
			globalLightPaths.push_back(FullPath(light->getPosition(), light->getMaterialId(), {}, {}));
            for (int j = 0; j < (int)rpl; j++)
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
				// printf("xy\n");
                for (FullPath &camPath : *pixelPaths)
                {
					// printf("CamPath\n");
                    for (int i = 0; i < globalLightPaths.size(); i++)
                    {
						FullPath &lightPath = globalLightPaths.at(i);
						FullPath combinedPaths = pathTracer.combine(camPath, lightPath, scene, background, x == 49 && y == 70);
						accumulatedColor += pathTracer.getColor(combinedPaths, scene);
                    }
                }
                floatBuffer.at(x, y) = accumulatedColor;
                // floatBuffer.at(x, y) = accumulatedColor / (pixelPaths->size() * globalLightPaths.size());
            }
        }
		
		floatBuffer.at(49, 70) = Vector3(1, 1, 1) / 100;
		
		// float maxWhite = -INFINITY;
        // for (int y = 0; y < resY; y++) {
        //     for (int x = 0; x < resX; x++) {
		// 		Vector3 c = floatBuffer.at(x, y);
		// 		maxWhite = std::max(std::max(c[0], c[1]), std::max(c[2], maxWhite));
		// 	}
		// }
		// ExtendedReinhardToneMapper(maxWhite).apply(floatBuffer);
		GlobalToneMapper().apply(floatBuffer);
		
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
};

#endif
