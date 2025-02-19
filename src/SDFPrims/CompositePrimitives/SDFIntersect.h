#ifndef __SDFINTERSECT
#define __SDFINTERSECT

#include "SDFPrims/CompositePrimitives/CompositePrimitive.h"

class SDFIntersect : public CompositePrimitive
{
public:
  using CompositePrimitive::CompositePrimitive;

  float getSignedDistance(Vector3 rayOrigin) const override
  {
    return std::max(primA->getSignedDistance(rayOrigin), primB->getSignedDistance(rayOrigin));
  };
  bool isSDF() const override { return true; };
};

#endif