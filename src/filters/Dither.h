#pragma once

#include "Effect.h"
#include <vector>

class Dither : public Effect
{
    using Effect::Effect;
    
    float amount;
    Buffer<Vector3> ditherTexture;
    std::vector<Vector3> *pallette;
    // https://en.wikipedia.org/wiki/Ordered_dithering
    // c' = nearest_pallette_color(c + r X (M(x mod n, y mod n) - 0.5))
    // M = threshold map
    // "r is the amount of spread in color space. Assuming an RGB palette with 23N evenly distanced colors where each color (a triple of red, green and blue values) is represented by an octet from 0 to 255, one would typically choose r ≈ 255 N {\textstyle r\approx {\frac {255}{N}}}. (1⁄2 is again the normalizing term.) "
    // 

    public:
        Dither *init(std::vector<Vector3> *pallette) {
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
        }

        std::vector<Vector3>* coolerPallette() {
            std::vector<Vector3> *p = new std::vector<Vector3>();
            p->push_back(fromHex("4b296b"));
            p->push_back(fromHex("a72608"));
            p->push_back(fromHex("8F5644"));
            p->push_back(fromHex("77867f"));
            p->push_back(fromHex("87b37a"));
            p->push_back(fromHex("9ce37d"));
            return p;
        }

        Vector3 fromHex(char* hex) {
            int r = hexVal(hex[1]) + 16 * hexVal(hex[0]);
            int g = hexVal(hex[3]) + 16 * hexVal(hex[2]);
            int b = hexVal(hex[5]) + 16 * hexVal(hex[4]);
            return Vector3((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f);
        }

        int hexVal(char hexVal) {
            if(hexVal >= 'a') return hexVal - 'a' + 10;
            else if(hexVal >= 'A') return hexVal - 'A' + 10;
            else if(hexVal >= '0') return hexVal - '0';
            else return 0;
        }

        // 0 8 2 10
        // 12 4 14 6
        // 3 11 1 9
        // 15 7 13 5
        // usually you would pass a premade dither image over the image, multiplying the result, but we can't read images
        void thresholdMap4x4(Buffer<float> *target) {
            *target = Buffer<float>(4, 4);
            int map[] = {0, 8, 2, 10, 12,4, 14,6, 3,11, 1,9, 15,7, 13,5};
            int maxValue = 15;
            float n2 = 1.0f / 16.0f;

            for(int i = 0; i < 16; i++) {
                int x = i % 4;
                int y = (i / 4);
                float v = (map[i] - (0.5 * maxValue)) * n2;
                target->at(x,y) = v;
            }
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
            printColor(closestColorInPallette(Vector3(0.85f, 0.12f, 0.12f)));
            printf("\n");
            printColor(fromHex("cd0000"));
            // printColor(closestColorInPallette(fromHex("cd0000")));
            printf("\n");

            size_t resX = this->imageBuffer->getWidth();
            size_t resY = this->imageBuffer->getHeight();
            Buffer<float> thresholdMap = Buffer<float>(4,4);
            this->thresholdMap4x4(&thresholdMap);
            this->colorSpace = RGB;


            for(int y=0; y<resY; y++) {
                for(int x=0; x<resX; x++) {
                    // 1. subsample imageBuffer and dither texture
                    // 2. calculate luminocity / brightness
                    // 3. calculate 
                    Vector3 color = this->imageBuffer->at(x,y);
                    float thresholdVal = thresholdMap.at(x % 4, y % 4);
                    Vector3 thresholdColor = color + Vector3(thresholdVal, thresholdVal, thresholdVal);
                    Vector3 closestColor = closestColorInPallette(thresholdColor);

                    this->imageBuffer->at(x,y) = closestColor;
                }
            }
            this->colorSpace = this->child->colorSpace;
        }

        void printColor(Vector3 color) {
            printf("%f %f %f\n", color[0], color[1], color[2]);
        }
};
