#pragma once

#include "Effect.h"

class HSVConvert : public Effect
{
    using Effect::Effect;
    
    public:
        void _apply() override
        {
            size_t resX = this->imageBuffer->getWidth();
            size_t resY = this->imageBuffer->getHeight();

            for(int y=0; y<resY; y++) {
                for(int x=0; x<resX; x++) {
                    Vector3 rgbColor = this->imageBuffer->at(x,y);
                    Vector3 hsvColor = rgb2hsv(rgbColor);
                    this->imageBuffer->at(x,y) = rgbColor;
                }
            }
        }
};

