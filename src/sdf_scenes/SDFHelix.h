#ifndef __SDFHELIX
#define __SDFHELIX

#include "Scene.h"

Scene loadSDFHelixScene() {
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
    Helix *helix = new Helix(0.5, 0.4, 0.1);
    helix->setMaterialId(0);
    primArray->add(helix);
	scene.setRootPrimitive(primArray);
    return scene;
}

#endif