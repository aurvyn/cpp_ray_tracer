#pragma once

#include "Buffer.h"
#include <list>
#include "HSV.h"

class Effect
{
    public:
        virtual void applyEffect(Buffer<Vector3> &floatBuffer) = 0;
};


class HueShift : public Effect
{
    float amount;
    public:
        HueShift(float amount)
        {
            this->amount = amount;
        }
        virtual void applyEffect(Buffer<Vector3> & floatBuffer)
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

// class HSVConvert : public Effect
// {
//     public:
//         virtual void apply(Buffer<Vector3> & floatBuffer)
//         {
//             size_t resX = floatBuffer.getWidth();
//             size_t resY = floatBuffer.getHeight();

//             for(int y=0; y<resY; y++)
//             {
//                 for(int x=0; x<resX; x++)
//                 {
//                     Vector3 rgbColor = floatBuffer.at(x,y);
//                     Vector3 hsvColor = rgbToHsv(rgbColor);
//                     floatBuffer.at(x,y) = hsvColor;
//                 }
//             }
//         }
// };


// class RGBConvert : public Effect
// {
//     public:
//         virtual void apply(Buffer<Vector3> & floatBuffer)
//         {
//             size_t resX = floatBuffer.getWidth();
//             size_t resY = floatBuffer.getHeight();

//             for(int y=0; y<resY; y++)
//             {
//                 for(int x=0; x<resX; x++)
//                 {
//                     Vector3 hsvColor = floatBuffer.at(x,y);
//                     Vector3 rgbColor = hsv2rgb(rgbColor);
//                     floatBuffer.at(x,y) = rgbColor;
//                 }
//             }
//         }
// };

// class LinearHSVHDR : public Effect
// {
//     public:
//         virtual void apply(Buffer<Vector3> & floatBuffer)
//         {
//             float maxValue = 0.0f;
//             size_t resX = floatBuffer.getWidth();
//             size_t resY = floatBuffer.getHeight();

//             for(int y=0; y<resY; y++)
//             {
//                 for(int x=0; x<resX; x++)
//                 {
//                     maxValue = std::max(floatBuffer.at(x,y)[2], maxValue);
//                 }
//             }

//             if(maxValue <= 1.0f)
//                 maxValue = 1.0f;
            
//             float toneMappingScale = 1.0f / maxValue;

//             for(int y=0; y<resY; y++)
//             {
//                 for(int x=0; x<resX; x++)
//                 {
//                     Vector3 toneMappedHSV = floatBuffer.at(x,y);
//                     toneMappedHSV[2] *= toneMappingScale;
//                     floatBuffer.at(x,y) = toneMappedHSV;
//                 }
//             }
//         }
// };