#ifndef __COMPOSITE_PRIMITIVE
#define __COMPOSITE_PRIMITIVE

#include "Primitive.h"
#include <math.h>

class CompositePrimitive : public Primitive
{
public:
    CompositePrimitive(Primitive *primA, Primitive *primB) : primA(primA), primB(primB) {}

    Vector3 getBBMin() const
    {
        Vector3 minA = primA->getBBMin();
        Vector3 minB = primA->getBBMin();

        return Vector3(std::min(minA[0], minB[0]), std::min(minA[1], minB[1]), std::min(minA[2], minB[2]));
    }
    Vector3 getBBMax() const
    {
        Vector3 maxA = primA->getBBMin();
        Vector3 maxB = primA->getBBMin();

        return Vector3(std::max(maxA[0], maxB[0]), std::max(maxA[1], maxB[1]), std::max(maxA[2], maxB[2]));
    }

    Vector3 getSDFNorm(Vector3 rayOrigin) const override
    {
        const float h = 0.0001;
        const Vector3 xyy = Vector3(1, -1, -1);
        const Vector3 yyx = Vector3(-1, -1, 1);
        const Vector3 yxy = Vector3(-1, 1, -1);
        const Vector3 xxx = Vector3(1, 1, 1);
        return (xyy * getSignedDistance(rayOrigin + xyy * h) +
                yyx * getSignedDistance(rayOrigin + yyx * h) +
                yxy * getSignedDistance(rayOrigin + yxy * h) +
                xxx * getSignedDistance(rayOrigin + xxx * h))
            .normalize();
    }

protected:
    Primitive *primA, *primB;
};

#endif