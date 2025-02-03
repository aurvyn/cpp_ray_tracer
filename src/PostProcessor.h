#pragma once

#include "Buffer.h"
#include <list>
#include "HSV.h"
#include "Effect.h"

// TODO: Have apply return a Buffer<Vector3> so you can debug write each step in the pipeline to an image?
class PostProcessor
{
    std::list<Effect*> effects = std::list<Effect*>();
    public:
        PostProcessor() {}
        PostProcessor(std::list<Effect*> &effects) {
            this->effects = effects;
        }

        void process(Buffer<Vector3> &imageBuffer) {
            for(Effect *effect: effects) {
                effect->applyEffect(imageBuffer);
            }
        }
};

class DefaultPipeline 
{
    public:
        PostProcessor pp;
        DefaultPipeline() {
            std::list<Effect*> effects = std::list<Effect*>();
            // effects.push_front(HSVConvert());
            // TODO wont work
            auto hs = HueShift(20.0f);
            effects.push_back(&hs);
            // effects.push_back(LinearHSVHDR());
            // effects.push_back(RGBConvert());

            this->pp = PostProcessor(effects);
        }
};
