#pragma once

#include "Effect.h"

class RGBConvert : public Effect
{
    using Effect::Effect; // uses super constructor

    public:
        void _apply() override
        {
            this->colorSpace = RGB;
            size_t resX = this->imageBuffer->getWidth();
            size_t resY = this->imageBuffer->getHeight();

            for(int y=0; y<resY; y++) {
                for(int x=0; x<resX; x++) {
                    Vector3 hsvColor = this->imageBuffer->at(x,y);
                    // toneMappedHSV[2] *= toneMappingScale;
                    this->imageBuffer->at(x,y) = hsv2rgb(hsvColor);
                }
            }
        }
};
