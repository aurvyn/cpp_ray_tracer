#pragma once

#include "Buffer.h"
#include <list>
#include "HSV.h"

class Effect
{
    public:
        virtual void applyEffect(Buffer<Vector3> &floatBuffer) = 0;
};



