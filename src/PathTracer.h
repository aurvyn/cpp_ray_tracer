#pragma once

#include <vector>
#include <random>
#include <cmath>

#include "Ray.h"
#include "Hitpoint.h"
#include "Scene.h"
#include "Shader.h"
#include "GenQuaternion.h"
#include "math.h"

#define MAX_SPECULAR_THETA (M_PI/16)
#define DIFFUSE_GRANULARITY (50)
#define SPECULAR_GRANULARITY (5)

class Path {
private:
    HitDetails _hd;
    float _strength;
    
public:
    Path(HitDetails hd, float strength)
        : _hd(hd), _strength(strength) { }
    
    const HitDetails &hd() {
        return _hd;
    }
    
    const float strength() {
        return _strength;
    }
};

class PathTracer {
public:
	Vector3 getPointOnGreatArc(Vector3 const & axis, float theta, float phi) {
		Vector3 normal = axis.cross(Vector3(0, 1, 0));
		
		Quaternion thetaQuat(theta, normal); // Rotate outward by theta
		Quaternion phiQuat(phi, axis); // Rotate around by phi
		
		return (thetaQuat * phiQuat) * axis;
	}
    
    float getPathChance(Scene const &scene, HitDetails const &from, Vector3 const &dir) {
        float reflectance = scene.getMaterials()[from.materialId()].getReflectance();
        reflectance = clamp(0, 1, reflectance);
        
        // If the path is within the hemisphere, it's a uniform distribution. Otherwise, 0.
        float diffuseDot = dir.dot(from.normal());
        float diffuseChance = diffuseDot > 0 ? 1.0f / DIFFUSE_GRANULARITY : 0;
        if (reflectance == 0)
            return diffuseChance;
        diffuseChance *= 1 - reflectance;
        
        // If the path is within the lobe, it's more likely to be closer to the reflection. Otherwise, 0.
        float specularChance = 0;
        float theta = acos(dir.dot(from.reflection()));
        float specularCoef = theta / MAX_SPECULAR_THETA;
        if (specularCoef > 0) {
            float rootValue = 1 - specularCoef * SPECULAR_GRANULARITY;
            rootValue = clamp(0, SPECULAR_GRANULARITY - 0.0000001f, rootValue);
            float lowBound = (int)rootValue;
            float highBound = (int)rootValue + 1;
            lowBound *= lowBound;
            highBound *= highBound;
            specularChance = reflectance * (highBound - lowBound) / highBound;
        }
        
        return diffuseChance + specularChance;
    }

    std::vector<Path> combine(std::vector<Path> const &from, std::vector<Path> const &to, Scene const &scene, size_t lightMaterial) {
        if (from.empty() || to.empty())
            return std::vector<Path>();
    
        HitDetails fromHd = const_cast<Path&>(from.back()).hd();
        HitDetails toHd = const_cast<Path&>(to.back()).hd();
        Vector3 fromPos = fromHd.position();
        Vector3 toPos = toHd.position();
    
        Ray connectionRay(toPos - fromPos, fromPos + fromHd.normal() * RAY_JITTER_EPSILON);
        Vector3 dir = connectionRay.getDirection();

        Hitpoint hit;
        if (scene.getRootPrimitive()->intersect(connectionRay, hit))
        {
            float distance = (toPos - fromPos).length();
            if (hit.getParameter() < distance - RAY_JITTER_EPSILON)
                return std::vector<Path>();

            float strength = getPathChance(scene, fromHd, dir);
            std::vector<Path> newVec;

            for (int i = 0; i < from.size(); i++)
                newVec.push_back(from.at(i));

            HitDetails hd(connectionRay, Hitpoint(distance, toHd.normal(), toHd.materialId()));

            newVec.push_back(Path(hd, strength));

            std::vector<Path> reversedTo = reversePath(to, scene, lightMaterial);

            for (int i = 0; i < reversedTo.size(); i++)
                newVec.push_back(reversedTo.at(i));

            return newVec;
        } 
        else
        {
            return std::vector<Path>();
        }
    }

    std::vector<Path> reversePath(std::vector<Path> const &to, Scene const &scene, size_t lightMaterial) {
        std::vector<Path> newPathVector;

        for (int i = to.size() - 1; i > 0; i--)
        {
            Path currentPath = to.at(i);
            Path nextPath = to.at(i - 1);

            HitDetails originalHd = currentPath.hd();
            HitDetails nextHd = nextPath.hd();

            Vector3 newDirection = originalHd.direction() * -1;
            Ray newRay = Ray(newDirection, originalHd.position());

            Hitpoint newHitpoint = Hitpoint(originalHd.getParameter(), nextHd.normal(), nextHd.materialId());

            HitDetails newHd = HitDetails(newRay, newHitpoint);
            Path newPath = Path(newHd, currentPath.strength());
        }

        Path lastPath = to.front();
        HitDetails lastHd = lastPath.hd();

        Vector3 lastDirection = lastHd.direction() * -1;
        Ray lastRay = Ray(lastDirection, lastHd.position());
        Hitpoint lastPoint;
        
        lastPoint.setParameter(lastHd.getParameter());
        lastPoint.setMaterialId(lightMaterial);

        HitDetails lastNewDetails = HitDetails(lastRay, lastPoint);

        Path lastNewPath = Path(lastNewDetails, lastPath.strength());

        newPathVector.push_back(lastNewPath);

        return newPathVector;
    }

    Ray nextRay(HitDetails const &hd, Scene const &scene) {
        const Material &mat = scene.getMaterials()[hd.materialId()];
        float percentSpecular = clamp(0, 1, mat.getReflectance());
        
        Vector3 dir;
        if (Shader::RandomFloat(0, 1) < percentSpecular) {
            // The bias towards pure reflection. 
            float specularCoef = 1 - sqrt(Shader::RandomFloat(0, 1));
            dir = getPointOnGreatArc(hd.reflection(), specularCoef * MAX_SPECULAR_THETA, Shader::RandomFloat(0, M_PI * 2));
        } else {
            dir = getPointOnGreatArc(hd.reflection(), Shader::RandomFloat(0, M_PI / 2), Shader::RandomFloat(0, M_PI * 2));
        }
        
        return Ray(dir, hd.position() + hd.normal() * RAY_JITTER_EPSILON);
    }
    
    std::vector<Path> trace(Ray start, Scene const &scene, int maxDepth) {
        std::vector<Path> paths;
        
        Hitpoint hit;
        while (scene.getRootPrimitive()->intersect(start, hit) && paths.size() < maxDepth) {
            HitDetails hd(start, hit);
            paths.push_back(Path(hd, 1.0f));
            start = nextRay(hd, scene);
        }
        
        return paths;
    }
    
    Vector3 getColor(std::vector<Path> paths, Scene const &scene) {
        if (paths.size() == 0) {
            return Vector3(0, 0, 0);
        }

        Path lightPath = paths.back();
        Vector3 newColor = scene.getMaterials().at(lightPath.hd().materialId()).getKd() * lightPath.strength();
        
        for (int i = paths.size() - 2; i >= 0; i--) {
            Path currentPath = paths.at(i);
            Material currentMaterial = scene.getMaterials().at(currentPath.hd().materialId());
            newColor = (newColor * currentMaterial.getKd() * currentPath.strength());
        }

        return newColor;
    }
    
};