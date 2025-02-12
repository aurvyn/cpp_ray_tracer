#ifndef __SDFPEANUT
#define __SDFPEANUT

#include "Scene.h"

Scene loadSDFPeanutScene() {
    Scene scene;
    scene.addDefaultLight();
    scene.addDefaultCamera();
    std::vector<Material> materials;
    Material material;
    material.setKa(Vector3(0.65, 0.65, 0.65));
    material.setKd(Vector3(0.65, 0.65, 0.65));
    material.setKs(Vector3(0.65, 0.65, 0.65));
    material.setShininess(1.0f);
    materials.push_back(material);
    scene.setMaterials(materials);
	PrimitiveArray *primArray = new PrimitiveArray();
    Sphere *s1 = new Sphere(Vector3(-0.35, 0.5, 0), 0.5);
	s1->setMaterialId(1);
	Sphere *s2 = new Sphere(Vector3(0.35, -0.35, 0), 0.5);
	s2->setMaterialId(1);
    Metablob *mb = new Metablob(s1, s2, 0.1);
	primArray->add(mb);
	scene.setRootPrimitive(primArray);
    return scene;
}

#endif