#ifndef __ALIASING
#define __ALIASING

#include "GenVector.h"
#include "Buffer.h"
#include "Camera.h"
#include "Scene.h"
#include "RayGenerator.h"
#include "PrimitiveArray.h"
#include "Shader.h"

#include <vector>
#include <cmath>
#include <numbers>
#include <cstdlib>
#include <ctime>

class Aliasing
{
public:
    Aliasing()
    {
        srand(static_cast<unsigned>(time(0)));
    }

    void gaussianFilterWithJitter(Scene &scene, size_t resX, size_t resY, unsigned char *outputImage)
    {
        Buffer<Color> imageBuffer = Buffer<Color>(resX, resY);
        Buffer<Vector3> floatBuffer = Buffer<Vector3>(resX, resY);

        RayGenerator generator = RayGenerator(scene.getCamera(), resX, resY);

        int kernelSize = 3; // * 3x3 kernel
        float sigma = 1.0f;
        std::vector<std::vector<float>> gaussianKernel(kernelSize, std::vector<float>(kernelSize));
        computeGaussianKernel(gaussianKernel, sigma, kernelSize);

        int samplesPerPixel = kernelSize * kernelSize;

        for (int y = 0; y < resY; y++)
        {
            for (int x = 0; x < resX; x++)
            {
                Vector3 totalColor(0, 0, 0);
                float totalWeight = 0.0f;

                for (int sx = 0; sx < kernelSize; sx++)
                {
                    for (int sy = 0; sy < kernelSize; sy++)
                    {
                        // Generate random jitter offsets in the range [-0.5, 0.5]
                        float jitterX = (static_cast<float>(rand()) / RAND_MAX - 0.5f);
                        float jitterY = (static_cast<float>(rand()) / RAND_MAX - 0.5f);

                        float offsetX = (sx + 0.5f + jitterX) / kernelSize;
                        float offsetY = (sy + 0.5f + jitterY) / kernelSize;

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

                        float weight = gaussianKernel[sx][sy];
                        totalColor += sampleColor * weight;
                        totalWeight += weight;
                    }
                }

                floatBuffer.at(x, y) = totalColor / totalWeight;
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

    void boxFilterWithJitter(Scene &scene, size_t resX, size_t resY, unsigned char *outputImage)
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
                        // Generate random jitter offsets in the range [-0.5, 0.5]
                        float jitterX = (static_cast<float>(rand()) / RAND_MAX - 0.5f);
                        float jitterY = (static_cast<float>(rand()) / RAND_MAX - 0.5f);

                        float offsetX = (sx + 0.5f + jitterX) / 3.0f;
                        float offsetY = (sy + 0.5f + jitterY) / 3.0f;

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

    void boxFilterAliasing(unsigned char *inputImage, unsigned char *outputImage, size_t resX, size_t resY)
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

        int kernelSize = 3; // Adjustable kernel size
        float sigma = 1.0f; // Standard deviation for Gaussian
        std::vector<std::vector<float>> gaussianKernel(kernelSize, std::vector<float>(kernelSize));

        computeGaussianKernel(gaussianKernel, sigma, kernelSize);

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

    void computeGaussianKernel(std::vector<std::vector<float>> &kernel, float sigma, int size)
    {
        constexpr float pi = static_cast<float>(M_PI);
        float gauss_total = 0.0f;
        int center = size / 2;

        for (int y = 0; y < size; y++)
        {
            for (int x = 0; x < size; x++)
            {
                int dx = center - x;
                int dy = center - y;

                float part1 = 1.0f / (2.0f * pi * sigma * sigma);
                float part2 = std::exp(-(dx * dx + dy * dy) / (2.0f * sigma * sigma));
                float weight = part1 * part2;

                kernel[y][x] = weight;
                gauss_total += weight;
            }
        }

        // normalize the kernel
        for (int y = 0; y < size; y++)
        {
            for (int x = 0; x < size; x++)
            {
                kernel[y][x] /= gauss_total;
            }
        }
    }
    void downsampleTrace(Scene &scene, size_t resX, size_t resY, unsigned char *outputImage)
    {
       
        size_t highResX = resX * 2;
        size_t highResY = resY * 2;
        Buffer<Vector3> floatBuffer(highResX, highResY);
        RayGenerator generator(scene.getCamera(), highResX, highResY);
    
        
        for (size_t y = 0; y < highResY; y++)
        {
            for (size_t x = 0; x < highResX; x++)
            {
                Ray ray = generator.getRay(x, y);
                Hitpoint hit;
                Vector3 color(0, 0, 0);
    
                if (scene.getRootPrimitive()->intersect(ray, hit))
                {
                    color = Shader::shade(ray, hit, scene);
                }
                else
                {
                    Vector3 rc = ray.getDirection();
                    color = Vector3(fabs(rc[0]), fabs(rc[1]), fabs(rc[2]));
                }
    
                floatBuffer.at(x, y) = color;
            }
        }
    
      
        Buffer<Vector3> downsampledBuffer(resX, resY);
    
       
        for (size_t y = 0; y < resY; y++)
        {
            for (size_t x = 0; x < resX; x++)
            {
                // Averaging blur 2x2
                Vector3 avgColor = (
                    floatBuffer.at(x * 2, y * 2) +
                    floatBuffer.at(x * 2 + 1, y * 2) +
                    floatBuffer.at(x * 2, y * 2 + 1) +
                    floatBuffer.at(x * 2 + 1, y * 2 + 1)
                ) * 0.25f; // Divide by 4 to get the mean
    
                downsampledBuffer.at(x, y) = avgColor;
            }
        }
    
      
        Buffer<Color> imageBuffer(resX, resY);
        toneMap(downsampledBuffer, imageBuffer);
    
     
        for (size_t y = 0; y < resY; y++)
        {
            for (size_t x = 0; x < resX; x++)
            {
                Vector3 v = downsampledBuffer.at(x, y) * 255.0f;
                Color c = Color(v[0], v[1], v[2]);
                imageBuffer.at(x, y) = c;
            }
        }
    
 
        unsigned char *renderBuffer = (unsigned char *)&imageBuffer.at(0, 0);
        for (size_t i = 0; i < resX * resY * 3; i++)
        {
            outputImage[i] = renderBuffer[i];
        }
    }
    
    void aliasTraceGaussian(Scene &scene, size_t resX, size_t resY, unsigned char *outputImage)
    {
        Buffer<Color> imageBuffer(resX, resY);
        Buffer<Vector3> floatBuffer(resX, resY);
        RayGenerator generator(scene.getCamera(), resX, resY);

        int kernelSize = 3; // Adjustable kernel size
        float sigma = 1.0f; // Standard deviation for Gaussian
        std::vector<std::vector<float>> gaussianKernel(kernelSize, std::vector<float>(kernelSize));

        computeGaussianKernel(gaussianKernel, sigma, kernelSize);

        int halfSize = kernelSize / 2; // Offset for kernel positioning

        for (int y = 0; y < resY; y++)
        {
            for (int x = 0; x < resX; x++)
            {
                Vector3 totalColor(0, 0, 0);
                float totalWeight = 0.0f;

                for (int sx = 0; sx < kernelSize; sx++)
                {
                    for (int sy = 0; sy < kernelSize; sy++)
                    {
                        float offsetX = (sx - halfSize + 0.5f) / kernelSize;
                        float offsetY = (sy - halfSize + 0.5f) / kernelSize;

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

                        // Apply Gaussian weight
                        float weight = gaussianKernel[sx][sy];
                        totalColor += sampleColor * weight;
                        totalWeight += weight;
                    }
                }

                // Normalize weighted sum

                floatBuffer.at(x, y) = totalColor / totalWeight;
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

    void aliasTraceBoxFilter(Scene &scene, size_t resX, size_t resY, unsigned char *outputImage)
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

    
//     void aliasTraceGaussianDownSampled(Scene &scene, size_t resX, size_t resY, unsigned char *outputImage)
// {
//     Buffer<Color> imageBuffer(resX, resY);
//     Buffer<Vector3> floatBuffer(resX, resY);
//     RayGenerator generator(scene.getCamera(), resX, resY);

//     int kernelSize = 3; // Gaussian kernel size
//     float sigma = 1.0f; // Standard deviation for Gaussian
//     std::vector<std::vector<float>> gaussianKernel(kernelSize, std::vector<float>(kernelSize));

//     computeGaussianKernel(gaussianKernel, sigma, kernelSize);

//     int halfSize = kernelSize / 2; // Offset for kernel positioning

//     for (int y = 0; y < resY; y++)
//     {
//         for (int x = 0; x < resX; x++)
//         {
//             Vector3 totalColor(0, 0, 0);
//             float totalWeight = 0.0f;

//             // Instead of rendering at 2x res and downsampling later, take 4 sub-pixel samples
//             for (int dx = 0; dx < 2; dx++)
//             {
//                 for (int dy = 0; dy < 2; dy++)
//                 {
//                     float offsetX = (dx + 0.5f) / 2.0f;
//                     float offsetY = (dy + 0.5f) / 2.0f;

//                     Ray ray = generator.getRay(2 * x + offsetX, 2 * y + offsetY);
//                     Hitpoint hit;
//                     Vector3 sampleColor(0, 0, 0);

//                     if (scene.getRootPrimitive()->intersect(ray, hit))
//                     {
//                         sampleColor = Shader::shade(ray, hit, scene);
//                     }
//                     else
//                     {
//                         Vector3 rc = ray.getDirection();
//                         sampleColor = Vector3(fabs(rc[0]), fabs(rc[1]), fabs(rc[2]));
//                     }

//                     totalColor += sampleColor;
//                 }
//             }
            
//             // Average the 4 samples to store in the lower-res buffer
//             floatBuffer.at(x, y) = totalColor / 4.0f;
//         }
//     }

//     toneMap(floatBuffer, imageBuffer);

//     for (int y = 0; y < resY; y++)
//     {
//         for (int x = 0; x < resX; x++)
//         {
//             Vector3 v = floatBuffer.at(x, y) * 255.0f;
//             Color c = Color(v[0], v[1], v[2]);
//             imageBuffer.at(x, y) = c;
//         }
//     }

//     unsigned char *renderBuffer = (unsigned char *)&imageBuffer.at(0, 0);
//     for (int i = 0; i < resX * resY * 3; i++)
//     {
//         outputImage[i] = renderBuffer[i];
//     }
// }

    
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