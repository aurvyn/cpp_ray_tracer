//
// Created by martina9 on 2/17/2025.
//
#ifndef SDF2DREVOLVE_H
#define SDF2DREVOLVE_H

#include "Primitive.h"
#include "Primitive2D.h"
#include <math.h>

class SDF2DRevolve : public Primitive {
public:
  SDF2DRevolve(Primitive2D* prim, Vector3 pos, float axisDst) : prim(prim), pos(pos), axisDst(axisDst) {}
  bool isSDF() const override { return true; };
  float getSignedDistance(Vector3 rayOrigin) const override {
    Vector3 ray = rayOrigin - pos;
    ray[2] -= axisDst;
    Vector2 q = Vector2(Vector2(ray[0], ray[2]).length() - axisDst, ray[1]);
    return prim->getSignedDistance(q);
  }
  Vector3 getBBMin() const {
    Vector2 bbMin = prim->getBBMin();
    return {bbMin[0]-axisDst+pos[0],bbMin[1]+pos[1],bbMin[0]-axisDst+pos[2]};
  }
  Vector3 getBBMax() const {
    Vector2 bbMax = prim->getBBMax();
    return {bbMax[0]+axisDst+pos[0],bbMax[1]+pos[1],bbMax[0]+axisDst+pos[2]};
  }
protected:
  Primitive2D *prim;
  float axisDst;
  Vector3 pos;
};

#endif //SDF2DREVOLVE.H
