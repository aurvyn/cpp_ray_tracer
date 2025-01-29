#ifndef __RAYTRACER_H
#define __RAYTRACER_H

#include "GenVector.h"
#include "Buffer.h"
#include "Camera.h"
#include "Scene.h"
#include "RayGenerator.h"
#include "PrimitiveArray.h"
#include "Shader.h"
#include "HSV.h"

class RayTracer
{
public:
	void trace(Scene & scene, size_t resX, size_t resY, unsigned char * outputImage)
	{
		Buffer<Color> imageBuffer = Buffer<Color>(resX, resY);
		Buffer<Vector3> floatBuffer = Buffer<Vector3>(resX, resY);
		
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
					//floatBuffer.at(x,y) = Vector3(0.0f);
				}
				else
					floatBuffer.at(x,y) = rc;
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
				Vector3 rgbColor = floatBuffer.at(x,y);
				Vector3 hsvColor = rgbToHsv(rgbColor);
				maxValue = std::max(hsvColor[2], maxValue);
				floatBuffer.at(x,y) = hsvColor;
			}
		}

		if(maxValue <= 1.0f)
			maxValue = 1.0f;
		
		float toneMappingScale = 1.0f / maxValue;

		for(int y=0; y<resY; y++)
		{
			for(int x=0; x<resX; x++)
			{
				Vector3 toneMappedHSV = floatBuffer.at(x,y);
				toneMappedHSV[2] *= toneMappingScale;
				Vector3 toneMappedRGBColor = hsv2rgb(toneMappedHSV);
				floatBuffer.at(x,y) = toneMappedRGBColor;
			}
		}
	}
};

#endif

