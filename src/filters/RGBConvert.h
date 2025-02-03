#pragma once

#include "Effect.h"

class RGBConvert : public Effect
{
    public:
        void applyEffect(Buffer<Vector3> & floatBuffer) override
        {
            size_t resX = floatBuffer.getWidth();
            size_t resY = floatBuffer.getHeight();

            for(int y=0; y<resY; y++) {
                for(int x=0; x<resX; x++) {
                    Vector3 hsvColor = floatBuffer.at(x,y);
                    Vector3 rgbColor = hsv2rgb(rgbColor);
                    floatBuffer.at(x,y) = rgbColor;
                }
            }
        }
};
