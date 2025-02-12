#pragma once

#include "Effect.h"

// typedef std::function<Vector3(Vector3, Vector3)> PixelBlend;
typedef Vector3 (*PixelBlend)(Vector3, Vector3);
typedef Vector3 (*PixelEffect)(Vector3);
// typedef std::function<Vector3(Vector3)> PixelEffect;
typedef Vector3 (*PixelGenerator)(int x, int y);

class GeneratorEffect : public Effect {
    private: 
    PixelGenerator func;

    public:
    // passed in buffer_info is just to copy its attributes
    GeneratorEffect(PixelGenerator func, const Buffer<Vector3>* buffer_info) : Effect((Effect *)nullptr) {
        this->func = func;
        this->imageBuffer = new Buffer<Vector3>(buffer_info->getWidth(), buffer_info->getHeight());
        this->colorSpace = RGB;
    }

    void _apply() override {
        size_t resX = this->imageBuffer->getWidth();
        size_t resY = this->imageBuffer->getHeight();

        for(int y=0; y<resY; y++) {
            for(int x=0; x<resX; x++) {
                Vector3 got = this->func(x, y);
                this->imageBuffer->at(x,y) = got;
            }
        }
    }

};

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

GeneratorEffect *VSin(const Buffer<Vector3> * reference_buf) {
    return new GeneratorEffect([](int x, int y) {
        return Vector3((1 + sin(y/5.0))/2);
    }, reference_buf);
}

GeneratorEffect *HSin(const Buffer<Vector3> * reference_buf) {
    return new GeneratorEffect([](int x, int y) {
        return Vector3((1 + sin(x/5.0))/2);
    }, reference_buf);
}

BasicBlendMode *Add(Effect *layer1, Effect *layer2)  {
    return new BasicBlendMode([](Vector3 a, Vector3 b) {
        return a + b;
    }, layer1, layer2);
}

BasicBlendMode *Subtract(Effect *layer1, Effect *layer2)  {
    return new BasicBlendMode([](Vector3 a, Vector3 b) {
        return a - b;
    }, layer1, layer2);
}

BasicBlendMode *Lighten(Effect *layer1, Effect *layer2)  {
    return new BasicBlendMode([](Vector3 a, Vector3 b) {
        return Vector3(
            std::max(a[0], b[0]), 
            std::max(a[1], b[1]),
            std::max(a[2], b[0])
        );
    }, layer1, layer2);
}

BasicBlendMode *Darken(Effect *layer1, Effect *layer2)  {
    return new BasicBlendMode([](Vector3 a, Vector3 b) {
        return Vector3(
            std::min(a[0], b[0]), 
            std::min(a[1], b[1]),
            std::min(a[2], b[2])
        );
    }, layer1, layer2);
}



BasicBlendMode *Multiply(Effect *layer1, Effect *layer2)  {
    return new BasicBlendMode([](Vector3 a, Vector3 b) {
        return Vector3(a[0]*b[0], a[1]*b[1], a[2]*b[2]);
    }, layer1, layer2);
}

BasicBlendMode *Screen(Effect *layer1, Effect *layer2)  {
    return new BasicBlendMode([](Vector3 a, Vector3 b) {
        return Vector3(
            1 - (1 - a[0])*(1 - b[0]),
            1 - (1 - a[1])*(1 - b[1]),
            1 - (1 - a[2])*(1 - b[2])
        );
    }, layer1, layer2);
}

BasicBlendMode *Overlay(Effect *layer1, Effect *layer2)  {
    return new BasicBlendMode([](Vector3 a, Vector3 b) {
        return Vector3(
            (a[0] < 0.5) ? (2*a[0]*b[0]) : (1 - 2*(1 - a[0])*(1 - b[0])),
            (a[1] < 0.5) ? (2*a[1]*b[1]) : (1 - 2*(1 - a[1])*(1 - b[1])),
            (a[2] < 0.5) ? (2*a[2]*b[2]) : (1 - 2*(1 - a[2])*(1 - b[2]))
        );
    }, layer1, layer2);
}

BasicBlendMode *HardLight(Effect *layer1, Effect *layer2)  {
    return new BasicBlendMode([](Vector3 a, Vector3 b) {
        return Vector3(
            (b[0] < 0.5) ? (2*a[0]*b[0]) : (1 - 2*(1 - a[0])*(1 - b[0])),
            (b[1] < 0.5) ? (2*a[1]*b[1]) : (1 - 2*(1 - a[1])*(1 - b[1])),
            (b[2] < 0.5) ? (2*a[2]*b[2]) : (1 - 2*(1 - a[2])*(1 - b[2]))
        );
    }, layer1, layer2);
}

// BasicEffect *ConstantMultiply(Effect *layer1, float cutoff)  {
//     auto mul = [cutoff](Vector3 a) {
//         return a*cutoff;
//     };
//     return new BasicEffect(mul, layer1);
// }