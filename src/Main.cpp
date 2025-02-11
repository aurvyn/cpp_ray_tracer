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
#include "Shader.h"
#include "RayTracer.h"
#include "SDFIntersect.h"
#include "SDFUnion.h"
#include "SDFDifference.h"
#include "Metablob.h"
#include "Mandelbulb.h"
#include "JuliaSet.h"

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
			sdfRendering = true;

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

Scene getDefaultScene() {
	Scene scene;

	// FIXME: A lot of this stuff is never deleted
	// Memory leaks! YAY!!!!
	// Also shading doesn't work
	// This thing kinda sucks
	std::vector<Material> materials;
	Material testMat;
	testMat.setKa(0.2);
	testMat.setKd(0.5);
	testMat.setKs(0.1);
	testMat.setReflectance(0.0f);
	testMat.setTranslucency(0.0f);
	testMat.setShininess(0.0f);
	materials.push_back(testMat);
	Material testLight;
	testLight.setKa(10);
	testLight.setKd(10);
	testLight.setKs(10);
	testLight.setReflectance(0.0f);
	testLight.setTranslucency(0.0f);
	testLight.setShininess(0.0f);
	materials.push_back(testLight);
	scene.setMaterials(materials);

	std::vector<Light *> lights;
	Light *light = new Light();
	light->setPosition(Vector3(1, 0, 2));
	light->setMaterialId(1);
	scene.setLights(lights);

	PrimitiveArray *primArray = new PrimitiveArray();
	Sphere *s1 = new Sphere(Vector3(-.5, 0, 0), 1);
	s1->setMaterialId(0);
	Sphere *s2 = new Sphere(Vector3(.5, 0, 0), 1);
	s2->setMaterialId(0);
	Primitive *inter = new SDFDifference(s1, s2);
	inter->setMaterialId(0);
	primArray->add(inter);

	Camera cam = Camera(Vector3(0, 0, 3), Vector3(0, 0, 0), Vector3(0, 1, 0));

	scene.setRootPrimitive(primArray);
	scene.setCamera(cam);

	return scene;
}

int main(int argc, char **argv)
{
	getArgs(argc, argv);
	reportArgs();

	unsigned char *outputImage = (unsigned char *)malloc(resX * resY * 3 * sizeof(unsigned char));
	Scene scene = loadWithOBJLoader(scenePath);

	PrimitiveArray *primArray = new PrimitiveArray();
	Sphere *s1 = new Sphere(Vector3(-.5, 0, 0), 1);
	s1->setMaterialId(0);
	Sphere *s2 = new Sphere(Vector3(.5, 0, 0), 1);
	s2->setMaterialId(0);
	Primitive *inter = new SDFDifference(s1, s2);
	inter->setMaterialId(1);
	primArray->add(inter);
//    Mandelbulb *mandelbulb = new Mandelbulb(2.f);
//    primArray->add(mandelbulb);
    JuliaSet *juliaSet = new JuliaSet({0.3,0.3,0.3,0.3});
    primArray->add(juliaSet);


	Camera cam = Camera(Vector3(-0.3, 0, 1.6), Vector3(0, 0, -1), Vector3(0, 1, 0));

	scene.setRootPrimitive(primArray);
	scene.setCamera(cam);

	// Scene scene = getDefaultScene();

	RayTracer tracer;
	tracer.march(scene, resX, resY, outputImage);
	if (sdfRendering)
		tracer.march(scene, resX, resY, outputImage);
	else
		tracer.trace(scene, resX, resY, outputImage, false);

	simplePNG_write(outputPath, resX, resY, outputImage);

	printf("Done!\n");

	return 0;
}
