#pragma once

#include "Buffer.h"
#include <list>

class PostProcessor
{
    std::list<Effect> effects = std::list<Effect>();
    public:
        PostProcessor(std::list<Effect> &effects) {
            this->effects = effects;
        }

        void apply(Buffer<Color> &imageBuffer) {
            for(Effect effect : effects) {
                effect.apply(imageBuffer);
            }
        }
};

class Effect
{
    public:
        virtual void apply(Buffer<Color> &imageBuffer);
};

class HueShift : public Effect
{
    float amount;
    public:
        HueShift(float amount)
        {
            this->amount = amount;
        }
};