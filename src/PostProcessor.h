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

        void process() {
            root->applyEffect();
        }
};

class Pipeline
{
    public:
        virtual PostProcessor* buildPipeline(Buffer<Vector3>* imageBuffer) = 0;
};

class DefaultPipeline: public Pipeline
{
    public:
        PostProcessor* buildPipeline(Buffer<Vector3>* imageBuffer) override {
            Effect *effect = new RGBConvert(new HSVConvert(imageBuffer));

            return new PostProcessor(effect);
        }
};
