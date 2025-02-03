#pragma once

#include "Effect.h"

class HueShift : public Effect
{
    float amount;
    public:
        HueShift(float amount)
        {
            this->amount = amount;
        }
        void applyEffect(Buffer<Vector3> & floatBuffer) override
        {
            size_t resX = floatBuffer.getWidth();
            size_t resY = floatBuffer.getHeight();

            for(int y=0; y<resY; y++)
            {
                for(int x=0; x<resX; x++)
                {
                    Vector3 hsvColor = floatBuffer.at(x,y);
                    float h = hsvColor[0];
                    h = std::fmod(h + this->amount, 360);
                    hsvColor[0] = h;
                    floatBuffer.at(x,y) = hsvColor;
                }
            }
        }
};
