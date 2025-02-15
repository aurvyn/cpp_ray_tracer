#pragma once

#include "Effect.h"
#include <vector>
#include <functional>

class FloydDither : public Effect
{
    using Effect::Effect;
    
    float amount;
    std::vector<Vector3> *pallette = NULL;

    // https://en.wikipedia.org/wiki/Ordered_dithering
    // c' = nearest_pallette_color(c + r X (M(x mod n, y mod n) - 0.5))
    // M = threshold map
    // "r is the amount of spread in color space. Assuming an RGB palette with 23N evenly distanced colors where each color (a triple of red, green and blue values) is represented by an octet from 0 to 255, one would typically choose r ≈ 255 N {\textstyle r\approx {\frac {255}{N}}}. (1⁄2 is again the normalizing term.) "

    public:
        FloydDither *init(std::vector<Vector3> *pallette) {
            this->pallette = pallette;
            return this;
        }

        std::vector<Vector3>* basicPallette() {
            std::vector<Vector3> *p = new std::vector<Vector3>(512); // 8^3
            float scl = 1.0f / 3.0f; // scales [0,7] -> [0,1]
            for(int r = 0; r < 3; r++) {
                for(int g = 0; g < 3; g++) {
                    for(int b = 0; b < 3; b++) {
                        Vector3 c = Vector3((float)r * scl,(float)g * scl,(float)b * scl);
                        p->push_back(Vector3((float)r * scl,(float)g * scl,(float)b * scl));
                    }
                }
            }
            return p;
        }

        std::vector<Vector3>* coolerPallette() {
            std::vector<Vector3> *p = new std::vector<Vector3>();
            p->push_back(rgbFromHex("4b296b"));
            p->push_back(rgbFromHex("a72608"));
            p->push_back(rgbFromHex("8F5644"));
            p->push_back(rgbFromHex("87b37a"));
            p->push_back(rgbFromHex("9ce37d"));
            return p;
        }

        Vector3 closestColorInPallette(Vector3 color) {
            // assumes rgb
            float minDist = 9999999999;
            Vector3 closestColor = this->pallette->at(0);
            for(int i = 0; i < this->pallette->size(); i++) {
                Vector3 c = this->pallette->at(i);
                Vector3 diff = c - color;
                float dist = diff.squaredLength();
                if(dist < minDist) {
                    minDist = dist;
                    closestColor = c;
                }
            }
            return closestColor;
        }

        void _apply() override
        {
            if(this->child->colorSpace != RGB) {
                // TODO: convert automatically
                printf("Convert to RGB before calling dither!\n");
            }

            if(this->pallette == NULL) this->pallette = coolerPallette();
            float correctionKernel[4] = {0.0f};
            correctionKernel[0] = 7.0f/16.0f;
            correctionKernel[1] = 3.0f/16.0f;
            correctionKernel[2] = 5.0f/16.0f;
            correctionKernel[3] = 1.0f/16.0f;

            size_t resX = this->imageBuffer->getWidth();
            size_t resY = this->imageBuffer->getHeight();
            this->colorSpace = RGB;
            
            for(int y=0; y<resY; y++) {
                for(int x=0; x<resX; x++) {
                    // c = c + correctionMap.at(1,1)
                    // correctionMap.at(1,1) = correctionMap.at(1,2) + diff * correctionKernel[0]
                    // ...
                    Vector3 color = this->imageBuffer->at(x,y);
                    Vector3 quantizedColor = closestColorInPallette(color);
                    this->imageBuffer->at(x,y) = quantizedColor;
                    Vector3 err = color - quantizedColor;
                    if(x+1 < resX) this->imageBuffer->at(x+1, y) += err * correctionKernel[0];
                    if(x-1 > 0 && y + 1 < resY) this->imageBuffer->at(x+1, y) += err * correctionKernel[1];
                    if(y + 1 < resY) this->imageBuffer->at(x+1, y) += err * correctionKernel[2];
                    if(x+1 < resX && y + 1 < resY) this->imageBuffer->at(x+1, y) += err * correctionKernel[3];

                    // Vector3 
                    // Vector3 thresholdColor = color + Vector3(thresholdVal, thresholdVal, thresholdVal);
                    // Vector3 closestColor = closestColorInPallette(thresholdColor);

                    // this->imageBuffer->at(x,y) = closestColor;
                }
            }
            this->colorSpace = this->child->colorSpace;
        }

        void printColor(Vector3 color) {
            printf("%f %f %f\n", color[0], color[1], color[2]);
        }
};
