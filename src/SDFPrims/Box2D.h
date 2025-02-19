//
// Created by martina9 on 2/18/2025.
//

#ifndef BOX2D_H
#define BOX2D_H

#include "Primitive2D.h"
#define sign(x) (x > 0) ? 1 : ((x < 0) ? -1 : 0)

class Box2D : public Primitive2D {
public:
  Box2D(Vector2 size) : size(size){};
  float getSignedDistance(Vector2 pos) const override{
    pos -= Vector2(size[0]*0.5f,size[1]*0.5f);
    Vector2 d = Vector2(abs(pos[0]),abs(pos[1])) - size;
    Vector2 dpos = Vector2(fmax(0.0,d[0]),fmax(0.0,d[1]));
    return fmin(fmax(d[0],d[1]),0.0f) + dpos.length();
  };

    Vector2 getBBMax() const override {
        return size*-0.5f;
    };

    Vector2 getBBMin() const override {
        return size*0.5f;
    }
protected:
    Vector2 size;
};
#endif //BOX2D_H
