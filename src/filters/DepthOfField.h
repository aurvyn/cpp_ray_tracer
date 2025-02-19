//
// Created by Henry Morin on 2/12/25.
//

#ifndef DEPTHOFFIELD_H
#define DEPTHOFFIELD_H

#include "Effect.h"
#include "Buffer.h"
#include "GenVector.h"
#include "Bound.h"
#include "ToneMapHSV.h"
#include "HSVConvert.h"
#include "RGBConvert.h"
#include "BasicEffect.h"
#include "BasicConvolution.h"

class DepthOfField : public BlendMode {
    using BlendMode::BlendMode;

    float focalPoint = 0.59f;
    float focalWidth = 0.07f;
    int res = 30;
public:
    DepthOfField *init(int res) {
        this->res = res;
        return this;
    }

    DepthOfField *init(float focalPoint, float focalWidth) {
        this->focalPoint = focalPoint;
        this->focalWidth = focalWidth;
        return this;
    }

    void _apply() override
    {
        this->colorSpace = this->child->colorSpace;
        size_t resX = this->imageBuffer->getWidth();
        size_t resY = this->imageBuffer->getHeight();

        float width = 1.0f / res;

        Buffer<Vector3> combined (resX, resY);

        for (int pass = 0; pass < res; pass++) {
            Buffer<Vector3> d (*imageBuffer2);

            Effect* nearFiltered = new RGBConvert((new Bound(new ToneMapHSV(new HSVConvert(&d))))->init(width*pass, (width*pass + width), 2));
            nearFiltered->applyEffect();

            for (int y = 0; y < resY; y++){
                for (int x = 0; x < resX; x++){
                    if (d.at(x,y).squaredLength() != 0) {
                        d.at(x,y) = Vector3(1.0f);
                    }
                }
            }

            Buffer<Vector3> i (*imageBuffer);

            Effect* focusImageBlur = Multiply((new BasicConvolution(NoOp(&i)))->init(pass-(res/2)), NoOp(&d));
            focusImageBlur->applyEffect();

            // if (pass == 2) {
            //     for (int y = 0; y < resY; y++){
            //         for (int x = 0; x < resX; x++){
            //             this->imageBuffer->at(x,y) = d.at(x,y);
            //         }
            //     }
            // }

            Effect* image = Add(NoOp(&combined), NoOp(&i));
            image->applyEffect();
        }

        for (int y = 0; y < resY; y++){
            for (int x = 0; x < resX; x++){
                this->imageBuffer->at(x,y) = combined.at(x,y);
            }
        }

    }

};

#endif //DEPTHOFFIELD_H
