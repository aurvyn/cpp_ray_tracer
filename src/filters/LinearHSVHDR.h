#pragma once

#include "Effect.h"

class LinearHSVHDR : public Effect
{
    public:
        void applyEffect(Buffer<Vector3> & floatBuffer) override
        {
            float maxValue = 0.0f;
            size_t resX = floatBuffer.getWidth();
            size_t resY = floatBuffer.getHeight();

            for(int y=0; y<resY; y++){
                for(int x=0; x<resX; x++) {
                    maxValue = std::max(floatBuffer.at(x,y)[2], maxValue);
                }
            }

            if(maxValue <= 1.0f)
                maxValue = 1.0f;
            
            float toneMappingScale = 1.0f / maxValue;

            for(int y=0; y<resY; y++) {
                for(int x=0; x<resX; x++) {
                    Vector3 toneMappedHSV = floatBuffer.at(x,y);
                    toneMappedHSV[2] *= toneMappingScale;
                    floatBuffer.at(x,y) = toneMappedHSV;
                }
            }
        }
};