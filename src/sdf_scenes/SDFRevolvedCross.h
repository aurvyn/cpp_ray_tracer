#ifndef __SDFREVOLVEDCROSS_H
#define __SDFREVOLVEDCROSS_H

#include "SDFPrims/SDF2DRevolve.h"
#include "SDFPrims/SDF2DExtrude.h"
#include "SDFPrims/Cross.h"
#include "SDFPrims/Circle.h"
#include "Scene.h"

Scene loadSDFRevolvedCrossScene() {
    Scene scene;
    scene.addDefaultLight();
    scene.addDefaultCamera();
    std::vector<Material> materials;
    Material material;
    material.setKa(Vector3(0.5, 0.0, 0.0));
    material.setKd(Vector3(0.5, 0.0, 0.0));
    material.setKs(Vector3(0.5, 0.5, 0.5));
    material.setShininess(1.0f);
    materials.push_back(material);
    scene.setMaterials(materials);
	PrimitiveArray *primArray = new PrimitiveArray();
    // LOG_INFO("Creating circle.");
    // Circle *c = new Circle(1.0f);
    Cross *c = new Cross(Vector2(0.5,0.5),0.01f);
    Primitive* revol = new SDFExtrude(c, Vector3(0,0,0),0.5f);
    // Primitive* revol = new SDF2DRevolve(c,Vector3(0.0,0.0,-2.0),0.0f);
	revol->setMaterialId(0);
	primArray->add(revol);
	scene.setRootPrimitive(primArray);
    return scene;
}

#endif