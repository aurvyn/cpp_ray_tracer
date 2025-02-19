#include <stdio.h>

#include "GenVector.h"
#include "simplePNG.h"
#include "Buffer.h"
#include "Camera.h"
#include "Scene.h"
#include "ObjLoader.h"
#include "SimpleLoader.h"
#include "RayGenerator.h"
#include "PrimitiveArray.h"
#include "RayTracer.h"
#include "Shader.h"
#include "SDFPrims/SDFIntersect.h"
#include "SDFPrims/SDFUnion.h"
#include "SDFPrims/SDFDifference.h"
#include "SDFPrims/Metablob.h"
#include "SDFPrims/Mandelbulb.h"
#include "SDFPrims/JuliaSet.h"
#include "SDFPrims/Helix.h"
#include "SDFPrims/Donut.h"
#include "SDFPrims/2DPrims/SDF2DRevolve.h"
#include "SDFPrims/2DPrims/SDF2DExtrude.h"
#include "SDFPrims/2DPrims/CoolS.h"
#include "SDFPrims/2DPrims/Box2D.h"
#include "SDFPrims/2DPrims/Cross.h"
#include "SDFPrims/2DPrims/Circle.h"
#include "Displacer.h"
#include "SDFPrims/SDFTwist.h"
#include "SDFSceneLoader.h"

#define RES 100

bool usePrimitiveArray = true;
bool foundScene = false;
bool sdfRendering = false;
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
		else if (strncmp(argv[i], "-sdf", 4) == 0 ||
				 strncmp(argv[i], "-s", 2) == 0)
		{
			sdfRendering = true;
			usePrimitiveArray = true;
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

	unsigned char *outputImage = (unsigned char *)malloc(resX * resY * 3 * sizeof(unsigned char));
	Scene scene;
	RayTracer tracer;
	if (sdfRendering)
	{
		scene = loadWithSDFLoader(scenePath);
		tracer.march(scene, resX, resY, outputImage);
	}
	else
	{
		reportArgs();
		scene = loadWithOBJLoader(scenePath);
		tracer.trace(scene, resX, resY, outputImage);
	}

	simplePNG_write(outputPath, resX, resY, outputImage);

	printf("Done!\n");

	return 0;
}
