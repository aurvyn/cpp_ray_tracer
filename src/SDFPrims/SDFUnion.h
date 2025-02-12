#ifndef __SDFUNION
#define __SDFUNION

#include "SDFPrims/CompositePrimitive.h"

class SDFUnion : public CompositePrimitive
{
public:
  using CompositePrimitive::CompositePrimitive;

  float getSignedDistance(Vector3 rayOrigin) const override
  {
    return std::min(primA->getSignedDistance(rayOrigin), primB->getSignedDistance(rayOrigin));
  };
  bool isSDF() const override { return true; };
};

#endif