#ifndef __SDFMORPH
#define __SDFMORPH

#include "Scene.h"

Scene loadSDFMorphScene() {
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
    Donut *dn = new Donut(0.5, 0.2);
    Displacer *dp = new Displacer(dn, 1.0f);
    dp->setMaterialId(0);
	primArray->add(dp);
	scene.setRootPrimitive(primArray);
    return scene;
}

#endif