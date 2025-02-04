#include <stdio.h>
#include <algorithm>
#include "GenVector.h"
#include "simplePNG.h"
#include "Buffer.h"
#include "Camera.h"
#include "Scene.h"
#include "ObjLoader.h"
#include "SimpleLoader.h"
#include "RayGenerator.h"
#include "PrimitiveArray.h"
#include "Shader.h"
#include "RayTracer.h"


#define RES 100

bool usePrimitiveArray = false;
bool foundScene = false;
char const *defaultScene = "test.obj";
char const *defaultOutput = "test.png";
char const *scenePath = defaultScene;
char const *outputPath = defaultOutput;
size_t resX = RES;
size_t resY = RES;

void getArgs(int argc, char **argv)
{
	for (int i = 1; i < argc; i++)
	{
		if (strncmp(argv[i], "-linear", 7) == 0 ||
			strncmp(argv[i], "-l", 2) == 0)
			usePrimitiveArray = true;

		else if (strncmp(argv[i], "-res", 4) == 0 ||
				 strncmp(argv[i], "-r", 2) == 0)
		{
			resX = atoi(argv[++i]);
			resY = atoi(argv[++i]);
		}

		else if (!foundScene)
		{
			// must be scene name
			foundScene = true;
			scenePath = argv[i];
		}
		else
		{
			outputPath = argv[i];
			return;
		}
	}
}

void printUsage()
{
	printf("usage: trace [options] scene [output]\n");
	printf("\t-r x y\t set resolution\n");
	printf("\t-l\t linear intersect (very slow)\n");
}

void reportArgs()
{
	FILE *testF = NULL;
	testF = fopen(scenePath, "rb");
	if (testF == NULL)
	{
		printf("No scene loaded.\n");
		printUsage();
		exit(0);
	}
	fclose(testF);

	printf("%s: ", scenePath);

	// if(resX != RES || resY != RES)
	printf("res %ix%i ", resX, resY);
	if (usePrimitiveArray)
		printf("linear ray intersect (slow) ");
	printf("\n");
}

Scene loadWithOBJLoader(char const *path)
{
	ObjLoader loader;
	Scene scene;

	if (usePrimitiveArray)
		scene = loader.getScene<true>(path);
	else
		scene = loader.getScene(path);

	return scene;
}

Scene loadWithSimpleLoader(char const *path)
{
	// this function just extracts the vertices and face indices from an OBJ
	SimpleLoader loader;
	Scene scene;
	objLoader objData = objLoader();

	objData.load(path);
	std::vector<float> verts;
	std::vector<int> tris;

	for (size_t i = 0; i < objData.vertexCount; i++)
		for (size_t v = 0; v < 3; v++)
			verts.push_back(objData.vertexList[i]->e[v]);
	for (size_t i = 0; i < objData.faceCount; i++)
		for (size_t v = 0; v < 3; v++)
			tris.push_back(objData.faceList[i]->vertex_index[v]);

	if (usePrimitiveArray)
		scene = loader.getScene<true>(objData.vertexCount, objData.faceCount, &verts[0], &tris[0]);
	else
		scene = loader.getScene(objData.vertexCount, objData.faceCount, &verts[0], &tris[0]);

	return scene;
}

void applyAntiAliasing(unsigned char *inputImage, unsigned char *outputImage, size_t resX, size_t resY)
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
				outputImage[index + c] = (
					outputImage[index + c] +
					outputImage[index + c - 3] +
					outputImage[index + c + 3] +
					outputImage[index + c - newResX * 3] +
					outputImage[index + c + newResX * 3]
				) / 5;
			}
		}
	}
}



void applyAntiAliasing1(unsigned char *inputImage, unsigned char *outputImage, size_t resX, size_t resY)
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
        {1/16.0f, 1/8.0f, 1/16.0f},
        {1/8.0f,  1/4.0f, 1/8.0f},
        {1/16.0f, 1/8.0f, 1/16.0f}
    };

   
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
                outputImage[index + c] = static_cast<unsigned char>(std::clamp(newValue, 0.0f, 255.0f));
            }
        }
    }
}

int main(int argc, char **argv)
{
	getArgs(argc, argv);
	reportArgs();

	unsigned char *outputImage = (unsigned char *)malloc(resX * resY * 3 * sizeof(unsigned char));
	Scene scene = loadWithOBJLoader(scenePath);
	RayTracer tracer;
	tracer.trace(scene, resX, resY, outputImage);

	
	

	simplePNG_write(outputPath, resX, resY, outputImage);

	
	size_t newResX = resX * 2;
	size_t newResY = resY * 2;
	unsigned char *antiAliasedImage = (unsigned char *)malloc(newResX * newResY * 3 * sizeof(unsigned char));
	applyAntiAliasing(outputImage, antiAliasedImage, resX, resY);

	
	char antiAliasedOutputPath[256];
	snprintf(antiAliasedOutputPath, sizeof(antiAliasedOutputPath), "aa_%s", outputPath);
	simplePNG_write(antiAliasedOutputPath, newResX, newResY, antiAliasedImage);
	

	

	free(outputImage);
	free(antiAliasedImage);
	

	return 0;
}
