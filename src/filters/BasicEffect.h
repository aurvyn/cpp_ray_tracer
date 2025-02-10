#pragma once

#include "Effect.h"

// typedef std::function<Vector3(Vector3, Vector3)> PixelBlend;
typedef Vector3 (*PixelBlend)(Vector3, Vector3);
typedef Vector3 (*PixelEffect)(Vector3);

class BasicEffect : public Effect {
    public:
    PixelEffect func;

    BasicEffect(PixelEffect func, Effect *layer1) : Effect(layer1) {
        this->func = func;
    }

    void _apply() override
    {
        size_t resX = this->imageBuffer->getWidth();
        size_t resY = this->imageBuffer->getHeight();
        this->colorSpace = this->child->colorSpace;

        for(int y=0; y<resY; y++) {
            for(int x=0; x<resX; x++) {
                Vector3 layer1Color = this->imageBuffer->at(x,y);
                this->imageBuffer->at(x,y) = this->func(layer1Color);
            }
        }
    }
};


class BasicBlendMode : public BlendMode {
    public:
    PixelBlend func;

    BasicBlendMode(PixelBlend func, Effect *layer1, Effect *layer2) : BlendMode(layer1, layer2) {
        this->func = func;
    }

    void _apply() override
    {
        size_t resX = this->imageBuffer->getWidth();
        size_t resY = this->imageBuffer->getHeight();
        this->colorSpace = this->child->colorSpace;

        for(int y=0; y<resY; y++) {
            for(int x=0; x<resX; x++) {
                Vector3 layer1Color = this->imageBuffer->at(x,y);
                Vector3 layer2Color = this->imageBuffer2->at(x,y);
                this->imageBuffer->at(x,y) = this->func(layer1Color, layer2Color);
            }
        }
    }
};


BasicBlendMode *Add(Effect *layer1, Effect *layer2)  {
    return new BasicBlendMode([](Vector3 a, Vector3 b) {
        return a + b;
    }, layer1, layer2);
}

BasicBlendMode *Multiply(Effect *layer1, Effect *layer2)  {
    return new BasicBlendMode([](Vector3 a, Vector3 b) {
        return Vector3(a[0]*b[0], a[1]*b[1], a[2]*b[2]);
    }, layer1, layer2);
}

BasicEffect *CMultiply(Effect *layer1, float cutoff)  {
    auto mul = [](Vector3 a) {
        return a*25.0f;
    };
    return new BasicEffect(mul, layer1);
}
