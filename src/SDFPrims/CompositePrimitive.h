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

    bool isSDF() const override { return true; };

protected:
    Primitive *primA, *primB;
};

#endif