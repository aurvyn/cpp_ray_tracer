//
// Created by lowej2 on 2/14/2025.
//

#ifndef COOLS_H
#define COOLS_H

#include "Primitive2D.h"
#define sign(x) (x > 0) ? 1 : ((x < 0) ? -1 : 0)

class CoolS : public Primitive2D {
public:
  CoolS(float dim) : dim(dim) {};
  float getSignedDistance(Vector2 p) const override{
    float six = (p[1]<0.0) ? -p[0] : p[0];
    p[0] = abs(p[0]);
    p[1] = abs(p[1]) - 0.2;
    float rex = p[0] - fmin(round(p[0]/0.4),0.4);
    float aby = abs(p[1]-0.2)-0.6;

    float c1 = clamp(0.5*(six-p[1]),0.0,0.2);
    float c2 = clamp(0.5*(p[0]-aby),0.0,0.4);
    float d = Vector2(six - c1, -p[1] - c1).length();
    d = fmin(d, Vector2(p[0] - c2, -aby - c2).length());
    d = fmin(d, Vector2(rex, p[1] - clamp(p[1],0.0,0.4)).length());
//    float d = dot2(Vector2(six,-p[1])-clamp(0.5*(six-p[1]),0.0,0.2));
//    d = min(d,dot2(Vector2(p[0],-aby)-clamp(0.5*(p[0]-aby),0.0,0.4)));
//    d = min(d,dot2(Vector2(rex,p[1]  -clamp(p[1]          ,0.0,0.4))));

    float s = 2.0*p[0] + aby + abs(aby+0.4) - 0.4;
    return sqrt(d) * sign(s);
  };

    Vector2 getBBMax() const override {
        return Vector2(-0.4, -0.6);
    };

    Vector2 getBBMin() const override {
        return Vector2(0.4, 0.6);
    }
protected:
    float dim;
};
#endif //COOLS_H
