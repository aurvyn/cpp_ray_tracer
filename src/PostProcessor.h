#pragma once

#include "Buffer.h"
#include "HSV.h"
#include "Effect.h"
#include <memory>
#include <vector>

// TODO: Have apply return a Buffer<Vector3> so you can debug write each step in the pipeline to an image?
class PostProcessor
{
    std::vector<Effect*> effects = std::vector<Effect*>();
    public:
        PostProcessor() {}
        PostProcessor(std::vector<Effect*> &effects) {
            this->effects = effects;
        }
        // ~PostProcessor() {
        //     for(Effect* effect : effects) {
        //         free(effect);
        //     }
        // }

        void process(Buffer<Vector3> &imageBuffer) {
            for(int i = 0; i < effects.size(); i++) {
                Effect* effect = effects.at(i);
                effect->applyEffect(imageBuffer);
            }
        }
};

class DefaultPipeline 
{
    public:
        PostProcessor pp;
        DefaultPipeline() {
            std::vector<Effect*> effects = std::vector<Effect*>();
            // TODO wont work
            effects.push_back(new HSVConvert());
            // effects.push_back(new HueShift(20.0));
            // effects.push_back(new LinearHSVHDR());
            // effects.push_back(new RGBConvert());

            this->pp = PostProcessor(effects);
        }
};
