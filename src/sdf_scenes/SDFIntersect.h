#ifndef __SDFINTERSECTSCENE
#define __SDFINTERSECTSCENE

#include "SDFIntersect.h"
#include "Scene.h"

Scene loadSDFIntersectScene() {
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
    JuliaSet *juliaSet = new JuliaSet({0.3,0.3,0.3,0.3});
    juliaSet->setMaterialId(0);
	Sphere *s2 = new Sphere(Vector3(0, -99.5, 0), 100);
	s2->setMaterialId(0);
	Primitive *inter = new SDFIntersect(juliaSet, s2);
	inter->setMaterialId(1);
	primArray->add(inter);
	scene.setRootPrimitive(primArray);
    return scene;
}

#endif