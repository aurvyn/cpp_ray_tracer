#pragma once

#include "Effect.h"
#include <vector>
#include <functional>

std::vector<Vector3>* basicPallette() {
    std::vector<Vector3> *p = new std::vector<Vector3>(512); // 8^3
    int colors_per_channel = 5;
    float scl = 1.0f / (float) colors_per_channel; // scales [0,7] -> [0,1]
    for(int r = 0; r < colors_per_channel; r++) {
        for(int g = 0; g < colors_per_channel; g++) {
            for(int b = 0; b < colors_per_channel; b++) {
                Vector3 c = Vector3((float)r * scl,(float)g * scl,(float)b * scl);
                p->push_back(c);
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

std::vector<Vector3>* coolerPallette2() {
    std::vector<Vector3> *p = new std::vector<Vector3>();
    p->push_back(rgbFromHex("AFAFAF"));
    p->push_back(rgbFromHex("DBDBDB"));
    p->push_back(rgbFromHex("635758"));
    p->push_back(rgbFromHex("0A6F71"));
    p->push_back(rgbFromHex("172F96"));
    p->push_back(rgbFromHex("64057B"));
    p->push_back(rgbFromHex("652774"));
    return p;
}


std::vector<Vector3>* spheresPallette() {
    std::vector<Vector3> *p = new std::vector<Vector3>();
    p->push_back(hsvFromHex("E30000"));
    p->push_back(hsvFromHex("606060"));
    p->push_back(hsvFromHex("C65A5A"));
    p->push_back(hsvFromHex("008900"));
    p->push_back(hsvFromHex("598D59"));
    p->push_back(hsvFromHex("17FB17"));
    p->push_back(hsvFromHex("00020D"));
    p->push_back(hsvFromHex("0000CC"));
    return p;
}

class FloydDither : public Effect
{
    using Effect::Effect;
    
    bool useGlitch = false;
    std::vector<Vector3> *pallette = NULL;

    // https://en.wikipedia.org/wiki/Ordered_dithering
    // c' = nearest_pallette_color(c + r X (M(x mod n, y mod n) - 0.5))
    // M = threshold map
    // "r is the amount of spread in color space. Assuming an RGB palette with 23N evenly distanced colors where each color (a triple of red, green and blue values) is represented by an octet from 0 to 255, one would typically choose r ≈ 255 N {\textstyle r\approx {\frac {255}{N}}}. (1⁄2 is again the normalizing term.) "

    public:
        FloydDither *init(bool useGlitch) {
            this->useGlitch = useGlitch;
            return this;
        }
        FloydDither *setPallette(std::vector<Vector3> *pallette) {
            this->pallette = pallette;
            return this;
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

            if(this->pallette == NULL) this->pallette = basicPallette();
            float correctionKernel[4] = {0.0f};
            float correctionScl = 1.0f;
            correctionKernel[0] = correctionScl * 7.0f/16.0f;
            correctionKernel[1] = correctionScl * 3.0f/16.0f;
            correctionKernel[2] = correctionScl * 5.0f/16.0f;
            correctionKernel[3] = correctionScl * 1.0f/16.0f;

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
                    if(!useGlitch) {
                        Vector3 err = color - quantizedColor;
                        if(x+1 < resX) this->imageBuffer->at(x+1, y) += err * correctionKernel[0];
                        if(x-1 > 0 && y + 1 < resY) this->imageBuffer->at(x-1, y+1) += err * correctionKernel[1];
                        if(y + 1 < resY) this->imageBuffer->at(x, y+1) += err * correctionKernel[2];
                        if(x+1 < resX && y + 1 < resY) this->imageBuffer->at(x+1, y+1) += err * correctionKernel[3];    
                    } else {
                        Vector3 err = quantizedColor - color;
                        if(x+1 < resX) this->imageBuffer->at(x+1, y) += err * correctionKernel[0];
                        if(x-1 > 0 && y + 1 < resY) this->imageBuffer->at(x+1, y) += err * correctionKernel[1];
                        if(y + 1 < resY) this->imageBuffer->at(x+1, y) += err * correctionKernel[2];
                        if(x+1 < resX && y + 1 < resY) this->imageBuffer->at(x+1, y) += err * correctionKernel[3];

                    }
                    
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
