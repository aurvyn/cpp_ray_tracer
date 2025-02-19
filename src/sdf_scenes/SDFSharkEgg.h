#ifndef __SDFSHARKEGG
#define __SDFSHARKEGG

#include "Scene.h"

Scene loadSDFSharkEggScene() {
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
    SDFTwist *tw = new SDFTwist(dn, 1.2f);
    tw->setMaterialId(0);
	primArray->add(tw);
	scene.setRootPrimitive(primArray);
    return scene;
}

#endif