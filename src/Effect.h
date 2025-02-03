#pragma once

#include "Buffer.h"
#include <list>
#include "HSV.h"

/*
Effect ideas
High pass / low pass filters
Bloom (high pass filter, blur, add it back on)
Toon shading
Thick outlines via edge detection (requires depth / normal map)
Photoshop effects
    Overlay
    Add
    Subtract
    Multiply

Vignette
Dithering
Blur
Sharpness
Ambient Occlusion
Depth of field
*/


class Effect
{
    public:
        Effect *child = NULL;
        Buffer<Vector3> *imageBuffer = NULL;

        Effect(Effect *child) {
            this->child = child;
        }

        Effect(Buffer<Vector3> *baseImage) { // 
            this->imageBuffer = baseImage;
        }

        Buffer<Vector3> *applyEffect() {
            if(this->imageBuffer == NULL) {
                this->imageBuffer = this->child->applyEffect();
            }
            this->_apply();
            return this->imageBuffer;
        }
        virtual void _apply() = 0;

};



