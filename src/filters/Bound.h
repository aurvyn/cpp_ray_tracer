#ifndef HIGHPASS_H
#define HIGHPASS_H

#include "Effect.h"

class Bound : public Effect {
    using Effect::Effect;

    float min;
    float max;
    int component;
    float replaceMin;
    float replaceMax;
    public:
    Bound *init(float min, float max, int component, float replaceMin=0.0f, float replaceMax=0.0f) {
        this->min = min;
        this->max = max;
        this->component = component;
        this->replaceMin = replaceMin;
        this->replaceMax = replaceMax;
        return this;
    }

    void _apply() override
    {
        if(this->child) this->colorSpace = this->child->colorSpace;
        else this->colorSpace = RGB;
        size_t resX = this->imageBuffer->getWidth();
        size_t resY = this->imageBuffer->getHeight();
        for(int y=0; y<resY; y++) {
            for(int x=0; x<resX; x++) {
                Vector3 color = this->imageBuffer->at(x,y);
                float v = color[component];
                if (v > max)
                    color[component] = replaceMax;
                else if (v < min)
                    color[component] = replaceMin;
                else
                    color[component] = v;
                this->imageBuffer->at(x,y) = color;
            }
        }
    }

};

#endif //HIGHPASS_H
