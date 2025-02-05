#pragma once

#include "Effect.h"

class RGBMultiply : public Effect
{
    using Effect::Effect;

    public:
        float amount = 1;

        RGBMultiply* init(float amount) {
            this->amount = amount;
            return this;
        }

        void _apply() override
        {
            if(this->child && this->child->colorSpace != RGB) {
                printf("Wrong colorspace! Image must be RGB to use RGBMultiply \n");
                exit(1);
            } 
            this->colorSpace = this->child->colorSpace;
            size_t resX = this->imageBuffer->getWidth();
            size_t resY = this->imageBuffer->getHeight();

            for(int y=0; y<resY; y++) {
                for(int x=0; x<resX; x++) {
                    Vector3 rgbVal = this->imageBuffer->at(x,y) * amount;
                    this->imageBuffer->at(x,y) = rgbVal;
                }
            }
        }
};

