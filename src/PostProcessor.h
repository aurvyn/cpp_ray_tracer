#pragma once

#include "Buffer.h"
#include "HSV.h"
#include "Effect.h"
#include <memory>
#include <vector>
#include "filters/HSVConvert.h"
#include "filters/HueShift.h"
#include "filters/LinearHSVHDR.h"
#include "filters/RGBConvert.h"

// TODO: Have apply return a Buffer<Vector3> so you can debug write each step in the pipeline to an image?
class PostProcessor
{
    Effect *root;
    public:
        PostProcessor() {}
        PostProcessor(Effect *root) {
            this->root = root;
        }

        void process(Buffer<Vector3> &imageBuffer) {
            root->applyEffect();
        }
};

class DefaultPipeline 
{
    public:
        PostProcessor pp;
        DefaultPipeline(Buffer<Vector3>* imageBuffer) {
            Effect *effect = new RGBConvert(new HSVConvert(imageBuffer));

            this->pp = PostProcessor(effect);
        }
};
