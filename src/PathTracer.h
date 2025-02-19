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

#define MAX_SPECULAR_THETA (M_PI/32)

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

class FullPath {
private:
    Vector3 _startPoint;
    size_t _startMaterial;
    std::vector<HitDetails> _hits;
    std::vector<float> _strengths;

public:
    FullPath(Vector3 startPoint, size_t startMaterial, std::vector<HitDetails> hits, std::vector<float> strengths)
        : _startPoint(startPoint), _startMaterial(startMaterial), _hits(hits), _strengths(strengths) { }

    const Vector3 &startPoint() {
        return _startPoint;
    }

    const size_t startMaterial() {
        return _startMaterial;
    }

    const std::vector<HitDetails> &hits() {
        return _hits;
    }

    const std::vector<float> &strengths() {
        return _strengths;
    }
};

class PathTracer {
private:
    float random(unsigned int *seed, float min, float max) {
		float r = (float) rand_r(seed) / (float) RAND_MAX;
		return min + r * (max - min);
    }

public:
    Ray sampleLightRay(const Light &light, unsigned int *seed)
    {
        Vector3 dir(random(seed, -1, 1), random(seed, -1, 1), random(seed, -1, 1));
		dir.normalize();
        return Ray(dir, light.getPosition());
    }

	Vector3 getPointOnGreatArc(Vector3 const & axis, float theta, float phi) {
		Vector3 normal = axis.cross(Vector3(0, 1, 0));
		
		Quaternion thetaQuat(theta, normal); // Rotate outward by theta
		Quaternion phiQuat(phi, axis); // Rotate around by phi
		
		return (thetaQuat * phiQuat) * axis;
	}

	Vector3 getPointOnHemisphere(Vector3 const & axis, unsigned int *seed) {
        Vector3 dir(random(seed, -1, 1), random(seed, -1, 1), random(seed, -1, 1));
		dir.normalize();
        if (dir.dot(axis) < 0)
            dir = -dir;
        return dir;
	}
    
    float getPathChance(Scene const &scene, HitDetails const &from, Vector3 const &dir, bool print) {
        float reflectance = scene.getMaterials()[from.materialId()].getReflectance();
        reflectance = clamp(0, 1, reflectance);
        
        // If the path is within the hemisphere, it's a uniform distribution. Otherwise, 0.
        float diffuseDot = dir.dot(from.normal());
        if (print) printf("Diffuse dot: %.4f\n", diffuseDot);
        float diffuseChance = diffuseDot >= -RAY_JITTER_EPSILON ? 1.0f : 0;
        if (reflectance == 0)
            return diffuseChance;

        // If the path is within the lobe, it's more likely to be closer to the reflection. Otherwise, 0.
        float specularChance = 0;
        float specularDot = dir.dot(from.reflection());
        if (specularDot > 0) {
            float theta = acos(specularDot);
            if (theta <= MAX_SPECULAR_THETA) {
                specularChance = sin(MAX_SPECULAR_THETA);
            }
        }
        
        return ((1 - reflectance) * diffuseChance) + (reflectance * specularChance);
    }

    FullPath combine(FullPath &from, FullPath &to, Scene const &scene, Hitpoint &background, bool print) {
        if (print) {
            printf("From: ((%.2f, %.2f, %.2f), %ld) ->", from.startPoint()[0], from.startPoint()[1], from.startPoint()[2], from.startMaterial());
            for (int i = 0; i < from.hits().size(); i++) printf(" (%ld, %.2f)", from.hits()[i].materialId(), from.strengths()[i]);
            printf("\nTo:   ((%.2f, %.2f, %.2f), %ld) ->", to.startPoint()[0], to.startPoint()[1], to.startPoint()[2], to.startMaterial());
            for (int i = 0; i < to.hits().size(); i++) printf(" (%ld, %.2f)", to.hits()[i].materialId(), to.strengths()[i]);
            printf("\n");
        }
        
        if (from.hits().empty() && to.hits().empty()) {
            if (print) printf("Both were empty, returning background\n\n");
            return FullPath(from.startPoint(), from.startMaterial(), {HitDetails(Ray(from.startPoint(), to.startPoint() - from.startPoint()), background)}, {1});
        }
        
        HitDetails fromHd;
        Vector3 fromPos;
        if (from.hits().size() > 0) {
            fromHd = from.hits().back();
            fromPos = fromHd.position();
        } else {
            fromPos = from.startPoint();
            fromHd = HitDetails(Ray(Vector3(0, 0, 0), fromPos), Hitpoint(0, Vector3(0, 0, 0), from.startMaterial()));
        }

        HitDetails toHd;
        Vector3 toPos;
        if (to.hits().size() > 0) {
            toHd = to.hits().back();
            toPos = toHd.position();
        } else {
            toPos = to.startPoint();
            toHd = HitDetails(Ray(Vector3(0, 0, 0), toPos), Hitpoint(0, Vector3(0, 0, 0), to.startMaterial()));
        }
    
        Ray connectionRay(toPos - fromPos, fromPos + fromHd.normal() * RAY_JITTER_EPSILON);
        Vector3 dir = connectionRay.getDirection();

        Hitpoint hit;
        bool intersected = scene.getRootPrimitive()->intersect(connectionRay, hit);
        if (from.hits().empty() && !intersected) {
            if (print) printf("Camera didn't intersect, returning background\n\n");
            return FullPath(from.startPoint(), from.startMaterial(), {HitDetails(connectionRay, background)}, {1});
        }
            
        float distance = (toPos - fromPos).length();
        float strength = getPathChance(scene, fromHd, dir, print);
        if (print) printf("Strength: %.4f\n", strength);
        if (hit.getParameter() < distance - RAY_JITTER_EPSILON) {
            if (print) printf("Collided\n");
            strength = strength / std::max(1.0f, hit.getParameter() * distance);
        }
        
        // strength /= distance;
        // strength /= distance * distance;

        std::vector<HitDetails> newHits;
        std::vector<float> newStrengths;

        for (int i = 0; i < from.hits().size(); i++) {
            newHits.push_back(from.hits()[i]);
            newStrengths.push_back(from.strengths()[i]);
        }

        HitDetails hd(connectionRay, Hitpoint(distance, toHd.normal(), toHd.materialId()));

        newHits.push_back(hd);
        newStrengths.push_back(strength);

        if (to.hits().size() > 0) {
            FullPath reversedTo = reversePath(to, scene);

            for (int i = 0; i < reversedTo.hits().size(); i++) {
                newHits.push_back(reversedTo.hits()[i]);
                newStrengths.push_back(reversedTo.strengths()[i]);
            }
        }

        if (print) printf("Returning a path of length %ld\n\n", newHits.size());
        return FullPath(from.startPoint(), from.startMaterial(), newHits, newStrengths);
    }

    FullPath reversePath(FullPath &to, Scene const &scene) {
        std::vector<HitDetails> newHits;
        std::vector<float> newStrengths;

        for (int i = to.hits().size() - 1; i > 0; i--)
        {
            HitDetails originalHd = to.hits()[i];
            float originalStrength = to.strengths()[i];
            HitDetails nextHd = to.hits()[i - 1];

            Vector3 newDirection = originalHd.direction() * -1;
            Ray newRay = Ray(newDirection, originalHd.position());

            Hitpoint newHitpoint = Hitpoint(originalHd.getParameter(), nextHd.normal(), nextHd.materialId());

            HitDetails newHd = HitDetails(newRay, newHitpoint);

            newHits.push_back(newHd);
            newStrengths.push_back(originalStrength);
        }

        HitDetails lastHd = to.hits().front();
        float lastStrength = to.strengths().front();

        Vector3 lastDirection = lastHd.direction() * -1;
        Ray lastRay = Ray(lastDirection, lastHd.position());
        Hitpoint lastPoint;
        
        lastPoint.setParameter(lastHd.getParameter());
        lastPoint.setMaterialId(to.startMaterial());

        HitDetails lastNewDetails = HitDetails(lastRay, lastPoint);

        newHits.push_back(lastNewDetails);
        newStrengths.push_back(lastStrength);

        return FullPath(to.hits().back().position(), to.hits().back().materialId(), newHits, newStrengths);
    }

    Ray nextRay(HitDetails const &hd, Scene const &scene, unsigned int *seed) {
        const Material &mat = scene.getMaterials()[hd.materialId()];
        float percentSpecular = clamp(0, 1, mat.getReflectance());
        
        Vector3 dir;
        if (random(seed, 0, 1) < percentSpecular) {
            // The bias towards pure reflection. 
            float specularCoef = 1 - sqrt(random(seed, 0, 1));
            dir = getPointOnGreatArc(hd.reflection(), specularCoef * MAX_SPECULAR_THETA, random(seed, 0, M_PI * 2));
        } else {
            dir = getPointOnHemisphere(hd.reflection(), seed);
        }
        
        return Ray(dir, hd.position() + hd.normal() * RAY_JITTER_EPSILON);
    }
    
    std::vector<FullPath> trace(Ray start, Scene const &scene, int maxDepth, size_t startingMaterial, unsigned int *seed) {
        std::vector<FullPath> fullPaths;
        
        std::vector<HitDetails> hits;
        std::vector<float> strengths;
        Vector3 startingPos = start.getOrigin();
        
        Hitpoint hit;
        while (scene.getRootPrimitive()->intersect(start, hit) && hits.size() < maxDepth) {
            if (hits.size() != 0)
                fullPaths.push_back(FullPath(startingPos, startingMaterial, std::vector(hits), std::vector(strengths)));
            HitDetails hd(start, hit);
            hits.push_back(hd);
            strengths.push_back(1.0f);
            start = nextRay(hd, scene, seed);
        }
        
        if (hits.size() != 0)
            fullPaths.push_back(FullPath(startingPos, startingMaterial, hits, strengths));
        return fullPaths;
    }
    
    Vector3 getColor(FullPath paths, Scene const &scene, bool print) {
        if (paths.hits().size() == 0) {
            if (print) printf("Colorizing an empty path, returning black\n\n");
            return Vector3(0, 0, 0);
        }
        
        HitDetails lightHd = paths.hits().back();
        float lightStrength = paths.strengths().back();
        Vector3 newColor = scene.getMaterials()[lightHd.materialId()].getKd() * lightStrength;
        
        if (print) printf("Colorizing (%.4f, %.4f, %.4f)", newColor[0], newColor[1], newColor[2]);
        for (int i = paths.hits().size() - 2; i >= 0; i--) {
            HitDetails currentHd = paths.hits()[i];
            float currentStrength = paths.strengths()[i];
            Material currentMaterial = scene.getMaterials()[currentHd.materialId()];
            newColor = (newColor * currentMaterial.getKd() * currentStrength);
            if (print) printf(" --(%.4f)--> (%.4f, %.4f, %.4f)", paths.strengths()[i], newColor[0], newColor[1], newColor[2]);
        }

        if (print) printf("\n\n");
        return newColor;
    }
    
};