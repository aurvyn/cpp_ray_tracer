#ifndef __RAYTRACER_H
#define __RAYTRACER_H

#include <iostream>
#include "GenVector.h"
#include "Buffer.h"
#include "Camera.h"
#include "Scene.h"
#include "RayGenerator.h"
#include "PrimitiveArray.h"
#include "Shader.h"
#include "HSV.h"
#include "PostProcessor.h"

class RayTracer
{
public:
	Pipeline *pipeline = new DefaultPipeline();

	void trace(Scene & scene, size_t resX, size_t resY, unsigned char * outputImage)
	{
		Buffer<Color> imageBuffer = Buffer<Color>(resX, resY);
		Buffer<Vector3> floatBuffer = Buffer<Vector3>(resX, resY);
		Buffer<Vector2> motionBuffer = Buffer<Vector2>(resX, resY);
		
		RayGenerator generator = RayGenerator(scene.getCamera(), resX, resY);
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
				
				Vector3 rc = ray.getDirection();
				rc = Vector3(fabs(rc[0]), fabs(rc[1]), fabs(rc[2]));

				bool hitSomething = false;
				Hitpoint hit;
				hitSomething = scene.getRootPrimitive()->intersect(ray, hit);
				if(hitSomething) {
					Vector3 floatColor = Shader::shade(ray, hit, scene);
					floatBuffer.at(x,y) = floatColor;
					// floatBuffer.at(x,y) = Vector3(0.0f);
					Vector3 motion = hit.getMotion();
					motion.projectToPlane(-scene.getCamera().getW());
					motionBuffer.at(x,y) = motion;
				}
				else
					floatBuffer.at(x,y) = rc;
					motionBuffer.at(x,y) = Vector2(0.0f);
			}
		}
		PostProcessor *pp = pipeline->buildPipeline(&floatBuffer, &motionBuffer);
		pp->process();

		for(int y=0; y<resY; y++)
		{
			for(int x=0; x<resX; x++)
			{
				Vector3 v = floatBuffer.at(x,y);
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
};

#endif

