#ifndef __SDFINTERSECT
#define __SDFINTERSECT

#include "CompositePrimitive.h"

class SDFIntersect : public CompositePrimitive
{
public:
  using CompositePrimitive::CompositePrimitive;

  float getSignedDistance(Vector3 rayOrigin) const override
  {
    return std::max(primA->getSignedDistance(rayOrigin), primB->getSignedDistance(rayOrigin));
  };
};

#endif