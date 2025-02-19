#ifndef __SDFDIFFERENCE
#define __SDFDIFFERENCE

#include "SDFPrims/CompositePrimitives/CompositePrimitive.h"

class SDFDifference : public CompositePrimitive
{
public:
  using CompositePrimitive::CompositePrimitive;

  float getSignedDistance(Vector3 rayOrigin) const override
  {
    return std::max(primA->getSignedDistance(rayOrigin), -primB->getSignedDistance(rayOrigin));
  };
  bool isSDF() const override { return true; };
};

#endif