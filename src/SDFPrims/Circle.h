//
// Created by martina9 on 2/17/2025.
//

#ifndef CIRCLE_H
#define CIRCLE_H

#include "Primitive2D.h"
#define clamp(a,b,v) (v) < (a) ? (a) : ( (v) > (b) ? (b) : (v) )
#define sign(x) (x > 0) ? 1 : ((x < 0) ? -1 : 0)

class Circle : public Primitive2D {
public:
    Circle(float rad) : rad(rad) {};
    float getSignedDistance(Vector2 p) const override{
        return p.length() - rad;
    };

    Vector2 getBBMax() const override {
        return Vector2(-rad, rad);
    };

    Vector2 getBBMin() const override {
        return Vector2(-rad, rad);
    }
protected:
    float rad;
};
#endif //CIRCLE_H
