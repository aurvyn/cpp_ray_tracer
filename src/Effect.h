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
    Negative

Vignette
Dithering
Blur
Sharpness
Ambient Occlusion - when there is a discontinuity in the depth buffer (like a corner), the further object is a little darker
Depth of field

Blur shadows when they're far away from the light source
Normal buffer, depth buffer, shadow buffer
Deferred renderring
- split render into trace and shade
- trace creates a buffer of hit information
- as a postprocessor, shade each pixel
- this allows you to postprocess the normals, shadows, etc etc beforehand
- e.g. bend normals of a flat plane to make water


*/

typedef int COLORSPACE;
static const COLORSPACE RGB = 0;
static const COLORSPACE HSV = 1;

class Effect
{
    public:
        Effect *child = NULL;
        Buffer<Vector3> *imageBuffer = NULL;
        COLORSPACE colorSpace;

        Effect(Effect *child) {
            this->child = child;
        }

        Effect(Buffer<Vector3> *baseImage, COLORSPACE defaultColorSpace=RGB) { // 
            this->imageBuffer = baseImage;
            this->colorSpace = defaultColorSpace;
        }

        virtual Buffer<Vector3> *applyEffect() {
            if(this->imageBuffer == NULL) {
                this->imageBuffer = this->child->applyEffect();
            }
            this->_apply();
            return this->imageBuffer;
        }
        virtual void _apply() = 0;
};

class BlendMode : public Effect {
    Effect *layer2 = NULL;

    public:
        Buffer<Vector3> *imageBuffer2;
        BlendMode(Effect *layer1, Effect *layer2) : Effect(layer1) {
            this->layer2 = layer2;
        }

        Buffer<Vector3> *applyEffect() override {
            this->imageBuffer = this->child->applyEffect();
            this->imageBuffer2 = this->layer2->applyEffect();
            this->_apply();
            return this->imageBuffer;
        }
};
