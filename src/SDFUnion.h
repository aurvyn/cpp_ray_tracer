#ifndef __SDFUNION
#define __SDFUNION

#include "CompositePrimitive.h"

class SDFUnion : public CompositePrimitive
{
public:
  using CompositePrimitive::CompositePrimitive;

  float getSignedDistance(Vector3 rayOrigin) const override
  {
    return std::min(primA->getSignedDistance(rayOrigin), primB->getSignedDistance(rayOrigin));
  };
};

#endif