#ifndef __RAYTRACER_H
#define __RAYTRACER_H

#include "GenVector.h"
#include "Buffer.h"
#include "MotionBuffer.h"
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
		//Buffer<Vector3> visualMotions(resX, resY);
		MotionBuffer motionBuffer(resX, resY);
		Buffer<Vector3> normalBuffer = Buffer<Vector3>(resX, resY);
		Buffer<float> depthFloatBuffer = Buffer<float>(resX, resY);
		float depthMax = 0.0f;
		float focal_len = resY / 2 * tan(scene.getCamera().getFov() / 2);
		
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
					float depth = hit.getParameter();
					Vector3 motion = hit.getMotion() - scene.getCamera().getMotion();
					motion.projectToPlane(ray.getDirection());
					motion[1] = -motion[1];
					//Vector3 visualMotion = Vector3(fabs(motion[0]), fabs(motion[1]), fabs(motion[2]));
					//visualMotions.at(x,y) = visualMotion/visualMotion.length()*255;
					motionBuffer.at(x,y) = motion * focal_len / depth;
					normalBuffer.at(x,y) = hit.getNormal();
					depthFloatBuffer.at(x,y) = depth;
					if (depth > depthMax) {
						depthMax = depth;
					}
				}
				else {
					floatBuffer.at(x,y) = rc;
					motionBuffer.at(x,y) = Vector2(0.0f);
					normalBuffer.at(x,y) = Vector3(0.0f);
					depthFloatBuffer.at(x,y) = -1.0f;
				}
			}
		}

		Buffer<Vector3> depthBuffer = Buffer<Vector3>(resX, resY);

		for(int y=0; y<resY; y++) {
			for(int x=0; x<resX; x++) {
				if (depthFloatBuffer.at(x,y) < 0) {
					depthBuffer.at(x,y) = Vector3(1.0f);
				} else {
					depthBuffer.at(x,y) = Vector3((depthFloatBuffer.at(x,y) / depthMax));
				}
			}
		}

		PostProcessor *pp = pipeline->buildPipeline(&floatBuffer, &normalBuffer, &depthBuffer, &motionBuffer);
		Buffer<Vector3> *ppBuffer = pp->process();

		for(int y=0; y<resY; y++)
		{
			for(int x=0; x<resX; x++)
			{
				Vector3 v = ppBuffer->at(x,y);
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

