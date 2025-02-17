#pragma once

#include <functional>
#include "Effect.h"

typedef std::function<Vector3(Vector3, Vector3)> PixelBlend;
typedef std::function<Vector3(Vector3)> PixelEffect;
typedef std::function<Vector3(int, int)> PixelGenerator;
// typedef Vector3 (*PixelBlend)(Vector3, Vector3);
// typedef Vector3 (*PixelEffect)(Vector3);
// typedef std::function<Vector3(Vector3)> PixelEffect;
// typedef Vector3 (*PixelGenerator)(int x, int y);

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

    BasicEffect(PixelEffect func, Buffer<Vector3> *base_image, COLORSPACE ColorSpace=RGB) : Effect(base_image, RGB) {
        this->func = func;
    }

    void _apply() override
    {
        size_t resX = this->imageBuffer->getWidth();
        size_t resY = this->imageBuffer->getHeight();
        if (this->child != NULL) this->colorSpace = this->child->colorSpace;

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

GeneratorEffect *VSin(const Buffer<Vector3> * reference_buf, float freq=5.0f) {
    return new GeneratorEffect([freq](int x, int y) {
        return Vector3((1 + sin(y/freq))/2);
    }, reference_buf);
}

GeneratorEffect *HSin(const Buffer<Vector3> * reference_buf, float freq=5.0f) {
    return new GeneratorEffect([freq](int x, int y) {
        return Vector3((1 + sin(x/freq))/2);
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

BasicEffect *Negative(Effect *layer1) {
    return new BasicEffect([](Vector3 a) {
        return Vector3(1.0) - a;
    }, layer1);
}

BasicEffect *NoOp(Effect *layer1) {
    return new BasicEffect([](Vector3 a) {
        return a;
    }, layer1);
}

BasicEffect *NoOp(Buffer<Vector3>* layer1) {
    return new BasicEffect([](Vector3 a) {
        return a;
    }, layer1);
}

BasicEffect *ConstMultiply(Effect *layer1, float val)  {
    auto mul = [val](Vector3 a) {
        return a*val;
    };
    return new BasicEffect(mul, layer1);
}

BasicEffect *ColorMultiply(Effect *layer1, Vector3 color)  {
    auto mul = [color](Vector3 a) {
        return a*color;
    };
    return new BasicEffect(mul, layer1);
}


BasicEffect *ConstAdd(Effect *layer1, float val)  {
    auto add = [val](Vector3 a) {
        return a+val;
    };
    return new BasicEffect(add, layer1);
}


BasicEffect *ColorAdd(Effect *layer1, Vector3 val)  {
    auto add = [val](Vector3 a) {
        return a+val;
    };
    return new BasicEffect(add, layer1);
}

enum ThresholdOptions {
    RED   = 0,
    GREEN = 1,
    BLUE  = 2,
    AVG   = 3,
    MAX   = 4,
};

BasicEffect *Threshold(Effect *layer1, float cutoff, ThresholdOptions on, bool above=true)  {
    // above=true: include pixels above cutoff
    // above=false: include pixels below cutoff
    bool below = !above;
    if (on < 3) {
        return new BasicEffect([on, cutoff, below](Vector3 a) {
            return Vector3((a[on] > cutoff) ^ below);
        }, layer1);
    } else if (on == AVG) {
        return new BasicEffect([cutoff, below](Vector3 a) {
            return Vector3(((a[0] + a[1] + a[2])/3 > cutoff) ^ below);
        }, layer1);
    } else if (on == MAX) {
        return new BasicEffect([cutoff, below](Vector3 a) {
            return Vector3((std::max(std::max(a[0], a[1]), a[2]) > cutoff) ^ below);
        }, layer1);
    } else {
        return new BasicEffect([cutoff, below](Vector3 a) {
            return Vector3((std::max(std::max(a[0], a[1]), a[2]) > cutoff) ^ below);
        }, layer1);
    }
    // TODO what here
}