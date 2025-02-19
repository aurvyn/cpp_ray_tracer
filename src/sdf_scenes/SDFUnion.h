#ifndef __SDFUNIONSCENE
#define __SDFUNIONSCENE

#include "SDFUnion.h"
#include "Scene.h"

Scene loadSDFUnionScene() {
    Scene scene;
    scene.addDefaultLight();
    scene.addDefaultCamera();
    std::vector<Material> materials;
    Material material;
    materials.push_back(material);
    material.setKa(Vector3(0.5, 0.0, 0.0));
    material.setKd(Vector3(0.5, 0.0, 0.0));
    material.setKs(Vector3(0.5, 0.5, 0.5));
    material.setShininess(1.0f);
    materials.push_back(material);
    scene.setMaterials(materials);
	PrimitiveArray *primArray = new PrimitiveArray();
    Sphere *s1 = new Sphere(Vector3(-.25, 0, .25), .75);
	s1->setMaterialId(0);
	Sphere *s2 = new Sphere(Vector3(.25, 0, -.25), .75);
	s2->setMaterialId(0);
	Primitive *inter = new SDFUnion(s1, s2);
	inter->setMaterialId(1);
	primArray->add(inter);
	scene.setRootPrimitive(primArray);
    return scene;
}

#endif