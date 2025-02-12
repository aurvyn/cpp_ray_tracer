#pragma once

#include "Effect.h"
#include "HSV.h"

class ToneMapHSV : public Effect
{
    using Effect::Effect;
    
    public:
        void _apply() override
        {
            this->colorSpace = this->child->colorSpace;
            float maxValue = 0.0f;
            size_t resX = this->imageBuffer->getWidth();
            size_t resY = this->imageBuffer->getHeight();

            for(int y=0; y<resY; y++){
                for(int x=0; x<resX; x++) {
                    // Vector3 rgbVal = this->imageBuffer->at(x,y) * 255.0f;
                    // Vector3 hsvVal = rgb2hsv(rgbVal);
                    Vector3 hsvVal = this->imageBuffer->at(x,y);
                    maxValue = std::max(hsvVal[2], maxValue);
                    // this->imageBuffer->at(x,y) = hsvVal;
                }
            }

            printf("maxValue = %f\n", maxValue);

            if(maxValue <= 1.0f)
                maxValue = 1.0f;
            
            float toneMappingScale = 1.0f / maxValue;

            for(int y=0; y<resY; y++) {
                for(int x=0; x<resX; x++) {
                    Vector3 toneMappedHSV = this->imageBuffer->at(x,y);
                    toneMappedHSV[2] *= toneMappingScale;
                    this->imageBuffer->at(x,y) = toneMappedHSV;
                }
            }
            
        }
};