#ifndef __DONUT
#define __DONUT

#include "Primitive.h"

class Donut : public Primitive {
public:
    Donut(float tx, float ty) : tx(tx), ty(ty) {}

    virtual Vector3 getBBMin() const {return Vector3 (0,0,0);}
    virtual Vector3 getBBMax() const {return Vector3 (0,0,0);}

    float getSignedDistance(Vector3 rayOrigin) const override {
        float length_xz = sqrt(rayOrigin[0] * rayOrigin[0] + rayOrigin[2] * rayOrigin[2]);
        float q_x = length_xz - tx;
        float q_y = rayOrigin[1];
        return sqrt(q_x * q_x + q_y * q_y) - ty;
    }

    Vector3 getSDFNorm(Vector3 rayOrigin) const {
        float epsilon = 0.00001f;
        Vector3 v1 = {
                getSignedDistance({rayOrigin[0] + epsilon, rayOrigin[1], rayOrigin[2]}),
                getSignedDistance({ rayOrigin[0], rayOrigin[1] + epsilon, rayOrigin[2] }),
                getSignedDistance({ rayOrigin[0], rayOrigin[1], rayOrigin[2] + epsilon })
        };
        Vector3 v2 = {
                getSignedDistance({rayOrigin[0] - epsilon, rayOrigin[1], rayOrigin[2]}),
                getSignedDistance({ rayOrigin[0], rayOrigin[1] - epsilon, rayOrigin[2] }),
                getSignedDistance({ rayOrigin[0], rayOrigin[1], rayOrigin[2] - epsilon })
        };
        return (v1 - v2).normalize();
    }

private:
    float tx;
    float ty;
};

#endif
