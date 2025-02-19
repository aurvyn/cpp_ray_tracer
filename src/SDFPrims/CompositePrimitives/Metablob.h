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
    float h = fmax(k - std::abs(aDist - bDist), 0.0f) / k;
    return fmin(aDist, bDist) - h * h * k * (1.0 / 4.0);
  }

    bool isSDF() const override { return true; };
private:
  float tolerance;
};

#endif