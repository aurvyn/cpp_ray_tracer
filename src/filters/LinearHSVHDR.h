#pragma once

#include "Effect.h"

class LinearHSVHDR : public Effect
{
    using Effect::Effect;
    
    public:
        void _apply() override
        {
            float maxValue = 0.0f;
            size_t resX = this->imageBuffer->getWidth();
            size_t resY = this->imageBuffer->getHeight();

            for(int y=0; y<resY; y++){
                for(int x=0; x<resX; x++) {
                    maxValue = std::max(this->imageBuffer->at(x,y)[2], maxValue);
                }
            }

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