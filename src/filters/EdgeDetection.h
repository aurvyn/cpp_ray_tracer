//
// Created by Henry Morin on 2/17/25.
//

#ifndef EDGEDETECTION_H
#define EDGEDETECTION_H

#include "Effect.h"
#include "BasicConvolution.h"
#include "BasicEffect.h"

class EdgeDetection : public Effect {
    using Effect::Effect;

public:

    void _apply() override
    {
        this->colorSpace = this->child->colorSpace;
        size_t resX = this->imageBuffer->getWidth();
        size_t resY = this->imageBuffer->getHeight();

        auto *copyBuffer = new Buffer<Vector3>(*imageBuffer);

        Effect *effect = (new BasicConvolution (NoOp(imageBuffer)))->init(1, Vector3(1, 2, 1), Vector3(1, 0, -1));
        Effect *effect2 = (new BasicConvolution (NoOp(copyBuffer)))->init(1, Vector3(1, 0, -1), Vector3(1, 2, 1));
        Effect *ret = Add(effect, effect2);

        Buffer<Vector3> *retBuffer = ret->applyEffect();

        for (int y = 0; y < resY; y++){
            for (int x = 0; x < resX; x++){
                this->imageBuffer->at(x,y) = retBuffer->at(x,y);
            }
        }
    }


};


#endif //EDGEDETECTION_H
