#pragma once

#include "Effect.h"

class NoOp : public Effect
{
    using Effect::Effect;

    public:
        void _apply() override {
            if (!this->child) {
                this->colorSpace = RGB;
            } else {
                this->colorSpace = this->child->colorSpace;
            }
        }
};

