#ifndef __SDFJULIA
#define __SDFJULIA

#include "Scene.h"

Scene loadSDFJuliaScene() {
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
    JuliaSet *juliaSet = new JuliaSet({0.3,0.3,0.3,0.3});
    juliaSet->setMaterialId(0);
    primArray->add(juliaSet);
	scene.setRootPrimitive(primArray);
    return scene;
}

#endif