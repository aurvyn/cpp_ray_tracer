#ifndef __SDFDONUT
#define __SDFDONUT

#include "Scene.h"

Scene loadSDFDonutScene() {
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
    dn->setMaterialId(0);
	primArray->add(dn);
	scene.setRootPrimitive(primArray);
    return scene;
}

#endif