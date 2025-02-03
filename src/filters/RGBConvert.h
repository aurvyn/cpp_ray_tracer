#pragma once

#include "Effect.h"

class RGBConvert : public Effect
{
    using Effect::Effect; // uses super constructor

    public:
        void _apply() override
        {
            size_t resX = this->imageBuffer->getWidth();
            size_t resY = this->imageBuffer->getHeight();

            for(int y=0; y<resY; y++) {
                for(int x=0; x<resX; x++) {
                    Vector3 hsvColor = this->imageBuffer->at(x,y);
                    Vector3 rgbColor = hsv2rgb(rgbColor);
                    this->imageBuffer->at(x,y) = rgbColor;
                }
            }
        }
};
