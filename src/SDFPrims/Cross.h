//
// Created by martina9 on 2/17/2025.
//

#ifndef CROSS_H
#define CROSS_H

#include "Primitive2D.h"
#define sign(x) (x > 0) ? 1 : ((x < 0) ? -1 : 0)

class Cross : public Primitive2D {
public:
  Cross(Vector2 size, float corner_rad) : size(size), corner_rad(corner_rad){};
  float getSignedDistance(Vector2 pos) const override{
    pos = Vector2(abs(pos[0]),abs(pos[1]));
    pos = (pos[1]>pos[0]) ? pos[1] : pos[0];
    Vector2 q = pos - size;
    float k = fmax(q[1],q[0]);
    Vector2  w = (k>0.0) ? q : Vector2(size[1]-pos[0],-k);
    Vector2 wpos = Vector2(abs(w[0]), abs(w[1]));
    return sign(k)*wpos.length() + corner_rad;
  };

    Vector2 getBBMax() const override {
        return Vector2(-0.4, -0.6);
    };

    Vector2 getBBMin() const override {
        return Vector2(0.4, 0.6);
    }
protected:
    Vector2 size;
    float corner_rad;
};
#endif //COOLS_H
