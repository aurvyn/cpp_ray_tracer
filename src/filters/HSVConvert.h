#pragma once

#include "Effect.h"

class HSVConvert : public Effect
{
    using Effect::Effect;

    public:
        void _apply() override
        {
            printf("Running RGBToHSV!\n");
            size_t resX = this->imageBuffer->getWidth();
            size_t resY = this->imageBuffer->getHeight();

            for(int y=0; y<resY; y++){
                for(int x=0; x<resX; x++) {
                    Vector3 rgbVal = this->imageBuffer->at(x,y) * 255.0f;
                    Vector3 hsvVal = rgb2hsv(rgbVal);
                    // maxValue = std::max(hsvVal[2], maxValue);
                    this->imageBuffer->at(x,y) = hsvVal;
                }
            }
        }
};

