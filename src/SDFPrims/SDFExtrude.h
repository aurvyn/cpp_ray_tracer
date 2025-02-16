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
  SDFExtrude(Primitive2D* prim, float extDist) : prim(prim), extDist(extDist) {}
  bool isSDF() const override { return true; };
  float getSignedDistance(Vector3 rayOrigin) const override {
    float d = prim->getSignedDistance(Vector2(rayOrigin[0], rayOrigin[1]));
    Vector2 w = Vector2(d, abs(rayOrigin[2] - extDist ));
    return fmin(fmax(w[0], w[1]), 0.0f) + fmax(w.dot(w), 0.0f);
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
};
#endif //SDFEXTRUDE_H
