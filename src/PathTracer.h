#pragma once

#include <vector>

#include "Ray.h"
#include "Hitpoint.h"
#include "Scene.h"
#include "Shader.h"

class PathTracer {
public:
    static const float SPECULAR_LOBE_PHI = M_PI/2/16;

    Ray nextRay(Ray const &from, Hitpoint const &hit, Scene const &scene) {
        HitDetails hd(from, hit);
        const Material &mat = scene.getMaterials()[hd.materialId()];
        
        Ray ray;
		ray.setOrigin(hd.position() + hd.normal()*RAY_JITTER_EPSILON);
        float dt = Shader::RandomFloat(0, M_PI);
        float dr;
        
        float percentSpecular = clamp(0, 1, mat.getReflectance());
        if (Shader::RandomFloat(0, 1) < percentSpecular) {
            ray.setDirection(hd.reflection());
            dr = Shader::RandomFloat(0, SPECULAR_LOBE_PHI);
        } else {
            ray.setDirection(hd.normal());
            dr = Shader::RandomFloat(0, M_PI/2);
        }
        
        
    }

    std::vector<Hitpoint> trace(Ray start, Scene const &scene, int maxDepth) {
        std::vector<Hitpoint> hits;
        
        Hitpoint hit;
        while (scene.getRootPrimitive()->intersect(start, hit) && hits.size() < maxDepth) {
            hits.push_back(hit);
            start = nextRay(start, hit, scene);
        }
        
        return hits;
    }
    
};