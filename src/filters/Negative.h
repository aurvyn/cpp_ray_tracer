#pragma once

#include "Effect.h"

class Negative : public Effect
{
    using Effect::Effect;

    public:
        void _apply() override
        {
            this->colorSpace = this->child->colorSpace;
            size_t resX = this->imageBuffer->getWidth();
            size_t resY = this->imageBuffer->getHeight();
            this->colorSpace = this->child->colorSpace;
            if(this->child->colorSpace == RGB) {
                for(int y=0; y<resY; y++) {
                    for(int x=0; x<resX; x++) {
                        Vector3 rgbVal = this->imageBuffer->at(x,y);
                        this->imageBuffer->at(x,y) = Vector3(1 - rgbVal[0], 1 - rgbVal[1], 1 - rgbVal[1]);
                    }
                }
            } else if(this->child->colorSpace == HSV) {
                for(int y=0; y<resY; y++) {
                    for(int x=0; x<resX; x++) {
                        Vector3 hsvVal = this->imageBuffer->at(x,y);
                        hsvVal[0] = fmod(hsvVal[0] + 180.0f, 360.0f);
                        this->imageBuffer->at(x,y) = hsvVal;
                    }
                }
            }
            

            
        }
};

