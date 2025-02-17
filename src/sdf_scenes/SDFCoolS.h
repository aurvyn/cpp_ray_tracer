//
// Created by lowej2 on 2/15/2025.
//

#ifndef SDFCOOLS_H
#define SDFCOOLS_H

#include "Scene.h"

Scene loadSDFCoolSScene() {
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
    auto s = new CoolS(0.2);
    Primitive* ext = new SDFExtrude(s, Vector3(0,0,-2), 2);
    ext->setMaterialId(0);
    primArray->add(ext);
    scene.setRootPrimitive(primArray);
    return scene;
}
#endif //SDFCOOLS_H
