//
// Created by lowej2 on 2/14/2025.
//

#ifndef SDFEXTRUDE_H
#define SDFEXTRUDE_H
#include "Primitive.h"
#include "Primitive2D.h"
#include <math.h>

class SDFExtrude : public Primitive {
public:
  SDFExtrude(Primitive2D* prim, Vector3 pos, float extDist) : prim(prim), pos(pos), extDist(extDist) {}
  bool isSDF() const override { return true; };
  float getSignedDistance(Vector3 rayOrigin) const override {
    Vector3 ray = rayOrigin - pos;
    float d = prim->getSignedDistance(Vector2(ray[0], ray[1]));
    Vector2 w = Vector2(d, abs(ray[2]) - extDist);
    Vector2 wpos = Vector2(fmax(w[0],0.0f), fmax(w[1],0.0f));
    return fmin(fmax(w[0], w[1]), 0.0f) + wpos.length();
  }
  Vector3 getBBMin() const {
    Vector2 bbMin = prim->getBBMin();
    return {bbMin[0],bbMin[1],0.0f};
  }
  Vector3 getBBMax() const {
    Vector2 bbMax = prim->getBBMax();
    return {bbMax[0],bbMax[1],extDist};
  }
protected:
  Primitive2D *prim;
  float extDist;
  Vector3 pos;
};
#endif //SDFEXTRUDE_H
