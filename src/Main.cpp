#include <stdio.h>
#include <algorithm>
#include <cmath>
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
#include "Aliasing.h"

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

int main(int argc, char **argv)
{
	getArgs(argc, argv);
	reportArgs();

	unsigned char *outputImage = (unsigned char *)malloc(resX * resY * 3 * sizeof(unsigned char)); // Original output image
	Scene scene = loadWithOBJLoader(scenePath);
	RayTracer tracer;
	tracer.trace(scene, resX, resY, outputImage);

	size_t newResX = resX * 2;
	size_t newResY = resY * 2;
	
	unsigned char *antiAliasedImageEnd = (unsigned char *)malloc(newResX * newResY * 3 * sizeof(unsigned char)); // Box filter anti-aliased image
	unsigned char *antiAliasedImagePerPixel = (unsigned char *)malloc(newResX * newResY * 3 * sizeof(unsigned char)); // Per-pixel box filter anti-aliased image
	unsigned char *antiAliasedImageGuassianPerPixel = (unsigned char *)malloc(newResX * newResY * 3 * sizeof(unsigned char)); // Per-pixel Gaussian filter anti-aliased image
	unsigned char *antiAliasedImageJitterBoxPP = (unsigned char *)malloc(newResX * newResY * 3 * sizeof(unsigned char)); // Jittered box filter anti-aliased image
	unsigned char *antiAliasedImageJitterGaussianPP = (unsigned char *)malloc(newResX * newResY * 3 * sizeof(unsigned char)); // Jittered Gaussian filter anti-aliased image
	unsigned char *downSampling = (unsigned char *)malloc(resX/2 * resY/2 * 3 * sizeof(unsigned char)); // Downsampled image

	Aliasing alias;
	alias.boxFilterAliasing(outputImage, antiAliasedImageEnd, resX, resY); // Apply box filter anti-aliasing
	alias.aliasTraceBoxFilter(scene, newResX, newResY, antiAliasedImagePerPixel); // Trace scene with per-pixel box filter anti-aliasing
	alias.aliasTraceGaussian(scene, newResX, newResY, antiAliasedImageGuassianPerPixel); // Trace scene with per-pixel Gaussian filter anti-aliasing
	alias.boxFilterWithJitter(scene, newResX, newResY, antiAliasedImageJitterBoxPP); // Trace scene with jittered box filter anti-aliasing
	alias.gaussianFilterWithJitter(scene, newResX, newResY, antiAliasedImageJitterGaussianPP); // Trace scene with jittered Gaussian filter anti-aliasing
	alias.downsampleTrace(scene, resX/2, resY/2, downSampling); // Downsample the traced scene

	char antiAliasedOutputPath[256]; // Output path for box filter anti-aliased image
	char antiAliasedOutputPathPP[256]; // Output path for per-pixel box filter anti-aliased image
	char antiAliasedOutputPathPPG[256]; // Output path for per-pixel Gaussian filter anti-aliased image
	char antiAliasedOutputPathJitterBox[256]; // Output path for jittered box filter anti-aliased image
	char antiAliasedOutputPathGaussianBox[256]; // Output path for jittered Gaussian filter anti-aliased image
	char downSampledOutputPath[256]; // Output path for downsampled image
	
	snprintf(antiAliasedOutputPath, sizeof(antiAliasedOutputPath), "boxFilter_%s", outputPath); // Format output path for box filter anti-aliased image
	snprintf(antiAliasedOutputPathPP, sizeof(antiAliasedOutputPathPP), "traceBoxFilter_%s", outputPath); // Format output path for per-pixel box filter anti-aliased image
	snprintf(antiAliasedOutputPathPPG, sizeof(antiAliasedOutputPathPPG), "traceGaussian_%s", outputPath); // Format output path for per-pixel Gaussian filter anti-aliased image
	snprintf(antiAliasedOutputPathJitterBox, sizeof(antiAliasedOutputPathJitterBox), "jitterBox_%s", outputPath); // Format output path for jittered box filter anti-aliased image
	snprintf(antiAliasedOutputPathGaussianBox, sizeof(antiAliasedOutputPathGaussianBox), "jitterGaus_%s", outputPath); // Format output path for jittered Gaussian filter anti-aliased image
	snprintf(downSampledOutputPath, sizeof(downSampledOutputPath), "downSampled_%s", outputPath); // Format output path for downsampled image

	simplePNG_write(outputPath, resX, resY, outputImage); // Write original output image to file
	simplePNG_write(antiAliasedOutputPath, newResX, newResY, antiAliasedImageEnd); // Write box filter anti-aliased image to file
	simplePNG_write(antiAliasedOutputPathPP, newResX, newResY, antiAliasedImagePerPixel); // Write per-pixel box filter anti-aliased image to file
	simplePNG_write(antiAliasedOutputPathPPG, newResX, newResY, antiAliasedImageGuassianPerPixel); // Write per-pixel Gaussian filter anti-aliased image to file
	simplePNG_write(antiAliasedOutputPathJitterBox, newResX, newResY, antiAliasedImageJitterBoxPP); // Write jittered box filter anti-aliased image to file
	simplePNG_write(antiAliasedOutputPathGaussianBox, newResX, newResY, antiAliasedImageJitterGaussianPP); // Write jittered Gaussian filter anti-aliased image to file
	simplePNG_write(downSampledOutputPath, resX / 2, resY / 2, downSampling); // Write downsampled image to file

	free(outputImage); // Free memory for original output image
	free(antiAliasedImageEnd); // Free memory for box filter anti-aliased image
	free(antiAliasedImagePerPixel); // Free memory for per-pixel box filter anti-aliased image
	free(antiAliasedImageGuassianPerPixel); // Free memory for per-pixel Gaussian filter anti-aliased image
	free(antiAliasedImageJitterBoxPP); // Free memory for jittered box filter anti-aliased image
	free(antiAliasedImageJitterGaussianPP); // Free memory for jittered Gaussian filter anti-aliased image
	free(downSampling); // Free memory for downsampled image

	return 0;
}
