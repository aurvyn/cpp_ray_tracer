#ifndef __RAYTRACER_H
#define __RAYTRACER_H

#include "GenVector.h"
#include "Buffer.h"
#include "Camera.h"
#include "Scene.h"
#include "RayGenerator.h"
#include "PrimitiveArray.h"
#include "Shader.h"

#define MIN_RAYMARCH_STEP_SIZE 0.00001
#define MAX_RAYMARCH_STEPS 100

class RayTracer
{
public:
	void trace(Scene &scene, size_t resX, size_t resY, unsigned char *outputImage, bool march)
	{
		Buffer<Color> imageBuffer = Buffer<Color>(resX, resY);
		Buffer<Vector3> floatBuffer = Buffer<Vector3>(resX, resY);

		RayGenerator generator = RayGenerator(scene.getCamera(), resX, resY);
		for (int y = 0; y < resY; y++)
		{
			for (int x = 0; x < resX; x++)
			{
				Ray ray = generator.getRay(x, y);

				// Day2
				// Ray r = generator.getRay(x, y);
				// Vector3 d = r.getDirection()*255.0f;
				// Color c = Color( abs(d[0]), abs(d[1]), abs(d[2]) );
				// floatBuffer.at(x,y) = d;
				// continue;

				Vector3 rc = ray.getDirection();
				rc = Vector3(fabs(rc[0]), fabs(rc[1]), fabs(rc[2]));

				bool hitSomething = false;
				Hitpoint hit;
				if (march)
					hitSomething = ((PrimitiveArray *)scene.getRootPrimitive())->intersectSDF(ray, hit);
				else
					hitSomething = scene.getRootPrimitive()->intersect(ray, hit);
				if (hitSomething)
				{
					Vector3 floatColor = Shader::shade(ray, hit, scene, true);
					floatBuffer.at(x, y) = floatColor;
					// floatBuffer.at(x,y) = Vector3(0.0f);
				}
				else
					floatBuffer.at(x, y) = rc;
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

	// TODO: Bisection
	// TODO: BVH
	void march(Scene &scene, size_t resX, size_t resY, unsigned char *outputImage)
	{
		Buffer<Color> imageBuffer = Buffer<Color>(resX, resY);
		Buffer<Vector3> floatBuffer = Buffer<Vector3>(resX, resY);

		RayGenerator generator = RayGenerator(scene.getCamera(), resX, resY);
		for (int y = 0; y < resY; y++)
		{
			for (int x = 0; x < resX; x++)
			{
				Ray currentRay = generator.getRay(x, y);
				Vector3 rc = currentRay.getDirection();
				Vector3 origin = currentRay.getOrigin();
				rc = Vector3(fabs(rc[0]), fabs(rc[1]), fabs(rc[2]));
				bool hitSomething = false;
				Hitpoint hit;

				for (int i = 0; i < MAX_RAYMARCH_STEPS; i++)
				{
					float safeStepSize = ((PrimitiveArray *)scene.getRootPrimitive())->getSignedDistance(currentRay.getOrigin(), hit);
					if (safeStepSize < MIN_RAYMARCH_STEP_SIZE)
					{
						// TODO: Record hit point and normal
						hitSomething = true;
						break;
					}
					currentRay = rayStep(currentRay, safeStepSize);
				}

				if (hitSomething)
				{
					// use this instead of Shader::shade to show distance field
					// float appox_dist = -1 * (origin - currentRay.getOrigin()).length();
					// Vector3 floatColor = Vector3(appox_dist, appox_dist, appox_dist) ;

					Vector3 floatColor = Shader::shade(currentRay, hit, scene, true);
					floatBuffer.at(x, y) = floatColor;
				}
				else
					floatBuffer.at(x, y) = rc;
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

	Ray rayStep(Ray ray, float stepSize)
	{
		return Ray(ray.getDirection(), ray.pointAtParameter(stepSize));
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
				floatColor[0] = fabs(floatColor[0]);
				floatColor[1] = fabs(floatColor[1]);
				floatColor[2] = fabs(floatColor[2]);
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
};

#endif
