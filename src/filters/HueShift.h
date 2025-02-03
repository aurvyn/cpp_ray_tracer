#pragma once

#include "Effect.h"

class HueShift : public Effect
{
    using Effect::Effect;
    
    float amount;
    public:
        HueShift *init(float amount) {
            this->amount = amount;
            return this;
        }

        void _apply() override
        {
            size_t resX = this->imageBuffer->getWidth();
            size_t resY = this->imageBuffer->getHeight();

            for(int y=0; y<resY; y++) {
                for(int x=0; x<resX; x++) {
                    Vector3 hsvColor = this->imageBuffer->at(x,y);
                    float h = hsvColor[0];
                    h = std::fmod(h + this->amount, 360.0f);
                    hsvColor[0] = h;
                    this->imageBuffer->at(x,y) = hsvColor;
                }
            }
        }
};
