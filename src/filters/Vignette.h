#pragma once

#include "Effect.h"
#define clamp(a,b,v) ((v) < (a) ? (a) : ( (v) > (b) ? (b) : (v) ))
#define lerp(a, b, t) (a + ((b - a) * t))

class Vignette : public Effect
{
    using Effect::Effect; // uses super constructor

    float intensity = 1.0f;
    float roundness = 1.0f;
    float smoothness = 1.0f;
    Vector4 color = Vector4(0.f, 0.f, 0.f, 1.f);

    public:
        /*
        intensity: makes vignette "darker"
        roundness: makes edges of the vignette less sharp
        smoothness: ?
        */
        Effect* init(Vector4 vignetteColor, float intensity=1.f, float roundness=1.f, float smoothness=1.f) {
            this->intensity = intensity;
            this->roundness = roundness;
            this->smoothness = smoothness;
            this->color = vignetteColor;
            return this;
        }

        void _apply() override
        {
            this->colorSpace = this->child->colorSpace;
            // 100,100 - 50,50 = 50,50
            size_t resX = this->imageBuffer->getWidth();
            size_t resY = this->imageBuffer->getHeight();
            Vector2 res = Vector2((float)resX, (float)resY);
            Vector2 center = res / 2.0f;
            float scaleFactor = intensity / (center.length());

            for(int y=0; y<resY; y++) {
                for(int x=0; x<resX; x++) {
                    Vector2 d = (Vector2(x,y) - center) * scaleFactor; // normalized to [0,intensity]
                    d = Vector2(std::pow(d[0], roundness), std::pow(d[1], roundness));
                    float f = clamp(0.0f, 1.0f, 1.0f - d.squaredLength());
                    float vfactor = std::pow(
                        clamp(0, 1, 1.0f - d.squaredLength()),
                        smoothness);
                    Vector3 c = this->imageBuffer->at(x,y);
                    this->imageBuffer->at(x,y) = Vector3(lerp(color[0], c[0], vfactor), lerp(color[1], c[1], vfactor), lerp(color[2], c[2], vfactor));
                }
            }
        }
};