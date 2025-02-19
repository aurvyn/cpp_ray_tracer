#ifndef __SDFREVOLVEDBOX_H
#define __SDFREVOLVEDBOX_H

#include "Scene.h"

Scene loadSDFRevolvedBoxScene() {
    Scene scene;
    scene.addDefaultLight();
    scene.setCamera(Camera({-1, 2, -3}, Vector3(0,0,0), {0, 1, 0}));
    std::vector<Material> materials;
    Material material;
    material.setKa(Vector3(0.65, 0.65, 0.65));
    material.setKd(Vector3(0.65, 0.65, 0.65));
    material.setKs(Vector3(0.65, 0.65, 0.65));
    material.setShininess(1.0f);
    materials.push_back(material);
    scene.setMaterials(materials);
	PrimitiveArray *primArray = new PrimitiveArray();
    Box2D *b = new Box2D(Vector2(0.5f,0.25f));
    Primitive* revol = new SDF2DRevolve(b,Vector3(0.0,0.0,0.0),2.0f);
	revol->setMaterialId(0);
	primArray->add(revol);
	scene.setRootPrimitive(primArray);
    return scene;
}

#endif