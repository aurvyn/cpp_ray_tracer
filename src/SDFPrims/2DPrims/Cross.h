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
    Vector2 p = Vector2(abs(pos[0]),abs(pos[1]));
    p = (p[1] > p[0]) ? Vector2(p[1],p[0]) : p;
    Vector2 q = p - size;
    float k = fmax(q[1],q[0]);
    Vector2 w = (k>0.0) ? q : Vector2(size[1]-p[0],-k);
    Vector2 wpos = Vector2(fmax(0.0f,w[0]), fmax(0.0f,w[1]));
    return sign(k)*wpos.length() + corner_rad;
  };

    Vector2 getBBMax() const override {
        float p = -1*fmax(size[0],size[1]);
        return Vector2(p,p);
    };

    Vector2 getBBMin() const override {
        float p = fmax(size[0],size[1]);
        return Vector2(p,p);
    }
protected:
    Vector2 size;
    float corner_rad;
};
#endif //COOLS_H
