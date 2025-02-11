#ifndef __METABLOB
#define __METABLOB

#include "CompositePrimitive.h"
#include <utility>
#include <cmath>

class Metablob : public CompositePrimitive
{
public:
  Metablob(Primitive *primA, Primitive *primB, float tolerance) : CompositePrimitive(primA, primB), tolerance(tolerance) {}

  float getSignedDistance(Vector3 rayOrigin) const override
  {
    float aDist = primA->getSignedDistance(rayOrigin);
    float bDist = primB->getSignedDistance(rayOrigin);
    float k = tolerance * 4.0;
    float h = std::max(k - std::abs(aDist - bDist), 0.0f) / k;
    return std::min(aDist, bDist) - h * h * k * (1.0 / 4.0);
  }

private:
  float tolerance;
};

#endif