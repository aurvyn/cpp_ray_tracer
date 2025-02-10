#ifndef HIGHPASS_H
#define HIGHPASS_H

#include "Effect.h"

class Bound : public Effect {
    using Effect::Effect;

    float min;
    float max;
    int component;
public:
    Bound *init(float min, float max, int component) {
        this->min = min;
        this->max = max;
        this->component = component;
        return this;
    }

    void _apply() override
    {
        this->colorSpace = this->child->colorSpace;
        size_t resX = this->imageBuffer->getWidth();
        size_t resY = this->imageBuffer->getHeight();
        for(int y=0; y<resY; y++) {
            for(int x=0; x<resX; x++) {
                Vector3 color = this->imageBuffer->at(x,y);
                float v = color[component];
                if (v > max)
                    color[component] = 0;
                else if (v < min)
                    color[component] = 0;
                else
                    color[component] = v;
                this->imageBuffer->at(x,y) = color;
            }
        }
    }

};

#endif //HIGHPASS_H
