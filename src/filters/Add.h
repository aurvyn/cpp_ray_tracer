#pragma once

#include "Effect.h"

class Add : public BlendMode
{
    using BlendMode::BlendMode;
    
    public:
        void _apply() override
        {
            size_t resX = this->imageBuffer->getWidth();
            size_t resY = this->imageBuffer->getHeight();
            this->colorSpace = this->child->colorSpace;

            for(int y=0; y<resY; y++) {
                for(int x=0; x<resX; x++) {
                    Vector3 layer1Color = this->imageBuffer->at(x,y);
                    Vector3 layer2Color = this->imageBuffer2->at(x,y);
                    this->imageBuffer->at(x,y) = layer1Color + layer2Color;
                }
            }
        }
};

