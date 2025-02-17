#ifndef __SDFMANDLEBULB
#define __SDFMANDLEBULB

#include "Scene.h"

Scene loadSDFMandelbulbScene() {
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
    Mandelbulb *mandelbulb = new Mandelbulb(2.1f);
    mandelbulb->setMaterialId(0);
    primArray->add(mandelbulb);
	scene.setRootPrimitive(primArray);
    return scene;
}

#endif