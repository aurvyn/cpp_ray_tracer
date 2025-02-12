//
// Created by Henry Morin on 2/10/25.
//

#ifndef BASICCONVOLUTION_H
#define BASICCONVOLUTION_H

#include "Effect.h"
#include "GenVector.h"

class BasicConvolution : public Effect {
    using Effect::Effect;



    int steps = 1;
public:
    BasicConvolution *init(int steps) {
        this->steps = steps;
        return this;
    }

    void _apply() override
    {
        this->colorSpace = this->child->colorSpace;
        size_t resX = this->imageBuffer->getWidth();
        size_t resY = this->imageBuffer->getHeight();


        Vector3 blurSeperatedConvolution = Vector3(0.25f, 0.5f, 0.25f);

        for (int step = 0; step < this->steps; step++) {
            Buffer<Vector3> tempBuffer(*imageBuffer);
            for (int i=0; i<3; i++) {
                for(int y=0; y<resY; y++) {
                    for(int x=1; x<resX-1; x++) {
                        Vector3 colors(0.0f);
                        int index = 0;
                        for (int j = x-1; j < x+2; j++){
                            colors[index] = tempBuffer.at(j,y)[i];
                            index++;
                        }

                        float combined = colors.dot(blurSeperatedConvolution);
                        imageBuffer->at(x,y)[i] = combined;
                    }
                }
                for(int y=1; y<resY-1; y++) {
                    for(int x=0; x<resX; x++) {
                        Vector3 colors(0.0f);
                        int index = 0;
                        for (int k = y-1; k < y+2; k++){
                            colors[index] = tempBuffer.at(x,k)[i];
                            index++;
                        }

                        float combined = colors.dot(blurSeperatedConvolution) + imageBuffer->at(x,y)[i];
                        this->imageBuffer->at(x,y)[i] = combined;
                    }
                }
            }
        }

    }

};

#endif //BASICCONVOLUTION_H
