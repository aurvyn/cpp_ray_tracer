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
