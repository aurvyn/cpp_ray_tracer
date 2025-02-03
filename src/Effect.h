#pragma once

#include "Buffer.h"
#include <list>
#include "HSV.h"

/*
Effect ideas
High pass / low pass filters
Bloom
Toon shading
Thick outlines via edge detection (requires depth / normal map)
Photoshop effects
    Overlay
    Add
    Subtract
    Multiply

Vignette
Dithering
Depth of field
    

*/

class Effect
{
    public:
        virtual void applyEffect(Buffer<Vector3> &floatBuffer) = 0;
};



