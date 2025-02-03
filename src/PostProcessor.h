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
#include "filters/Add.h"
#include "filters/NoOp.h"

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
        
            Effect *effect = new RGBConvert(new LinearHSVHDR(
                (new HueShift(new HSVConvert(imageBuffer)))->init(60.0f)
                ));
            // Effect *effect = new NoOp(imageBuffer);
            // Buffer<Vector3> copy = Buffer<Vector3>(*imageBuffer);
            // Effect *effect2 =   new RGBConvert(
            //                         (new HueShift(
            //                             new HSVConvert(&copy)
            //                         ))->init(20.0f)
            //                     );
            // Effect *root = new Add(effect, effect2);

            return new PostProcessor(effect);
        }
};
