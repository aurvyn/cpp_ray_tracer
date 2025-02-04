#ifndef __ALIASING
#define __ALIASING

#include "GenVector.h"
#include "Buffer.h"
#include "Camera.h"
#include "Scene.h"
#include "RayGenerator.h"
#include "PrimitiveArray.h"
#include "Shader.h"

class Aliasing
{
public:
    void averageAliasing(unsigned char *inputImage, unsigned char *outputImage, size_t resX, size_t resY)
    {
        size_t newResX = resX * 2;
        size_t newResY = resY * 2;

        for (size_t y = 0; y < newResY; y++)
        {
            for (size_t x = 0; x < newResX; x++)
            {
                int oldX = x / 2;
                int oldY = y / 2;
                int index = (oldY * resX + oldX) * 3;
                int newIndex = (y * newResX + x) * 3;

                outputImage[newIndex] = inputImage[index];
                outputImage[newIndex + 1] = inputImage[index + 1];
                outputImage[newIndex + 2] = inputImage[index + 2];
            }
        }

        for (size_t y = 1; y < newResY - 1; y++)
        {
            for (size_t x = 1; x < newResX - 1; x++)
            {
                int index = (y * newResX + x) * 3;

                for (int c = 0; c < 3; c++)
                {
                    outputImage[index + c] = (outputImage[index + c] +
                                              outputImage[index + c - 3] +
                                              outputImage[index + c + 3] +
                                              outputImage[index + c - newResX * 3] +
                                              outputImage[index + c + newResX * 3]) /
                                             5;
                }
            }
        }
    }

    void guassianAliasing(unsigned char *inputImage, unsigned char *outputImage, size_t resX, size_t resY)

    {
        size_t newResX = resX * 2;
        size_t newResY = resY * 2;

        for (size_t y = 0; y < newResY; y++)
        {
            for (size_t x = 0; x < newResX; x++)
            {
                int oldX = x / 2;
                int oldY = y / 2;
                int index = (oldY * resX + oldX) * 3;
                int newIndex = (y * newResX + x) * 3;

                outputImage[newIndex] = inputImage[index];
                outputImage[newIndex + 1] = inputImage[index + 1];
                outputImage[newIndex + 2] = inputImage[index + 2];
            }
        }

        const float gaussianKernel[3][3] = {
            {1 / 16.0f, 1 / 8.0f, 1 / 16.0f},
            {1 / 8.0f, 1 / 4.0f, 1 / 8.0f},
            {1 / 16.0f, 1 / 8.0f, 1 / 16.0f}};

        for (size_t y = 1; y < newResY - 1; y++)
        {
            for (size_t x = 1; x < newResX - 1; x++)
            {
                int index = (y * newResX + x) * 3;

                for (int c = 0; c < 3; c++)
                {
                    float newValue = 0.0f;

                    for (int ky = -1; ky <= 1; ky++)
                    {
                        for (int kx = -1; kx <= 1; kx++)
                        {
                            int neighborX = x + kx;
                            int neighborY = y + ky;
                            int neighborIndex = (neighborY * newResX + neighborX) * 3;

                            newValue += gaussianKernel[ky + 1][kx + 1] * outputImage[neighborIndex + c];
                        }
                    }

                    // Set the new pixel value
                    outputImage[index + c] = newValue;
                }
            }
        }
    }

    void aliasTrace(Scene &scene, size_t resX, size_t resY, unsigned char *outputImage)
    {
        Buffer<Color> imageBuffer = Buffer<Color>(resX, resY);
        Buffer<Vector3> floatBuffer = Buffer<Vector3>(resX, resY);

        RayGenerator generator = RayGenerator(scene.getCamera(), resX, resY);
        int samplesPerPixel = 9;

        for (int y = 0; y < resY; y++)
        {
            for (int x = 0; x < resX; x++)
            {
                Vector3 totalColor(0, 0, 0);

                for (int sx = 0; sx < 3; sx++)
                {
                    for (int sy = 0; sy < 3; sy++)
                    {
                        float offsetX = (sx + 0.5f) / 2.0f;
                        float offsetY = (sy + 0.5f) / 2.0f;

                        Ray ray = generator.getRay(x + offsetX, y + offsetY);

                        Hitpoint hit;
                        Vector3 sampleColor(0, 0, 0);

                        if (scene.getRootPrimitive()->intersect(ray, hit))
                        {
                            sampleColor = Shader::shade(ray, hit, scene);
                        }
                        else
                        {
                            Vector3 rc = ray.getDirection();
                            sampleColor = Vector3(fabs(rc[0]), fabs(rc[1]), fabs(rc[2]));
                        }

                        totalColor += sampleColor;
                    }
                }

                floatBuffer.at(x, y) = totalColor / samplesPerPixel;
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
        {
            for (int x = 0; x < resX; x++)
            {
                floatBuffer.at(x, y) = floatBuffer.at(x, y) * toneMappingScale;
            }
        }
    }
};

#endif