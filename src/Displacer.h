#ifndef __DISPLACER
#define __DISPLACER

#include "Primitive.h"

class Displacer : public Primitive {
public:
    Displacer(Primitive* primitive, float displacement) : prim(primitive), displacement(displacement) {}

    virtual Vector3 getBBMin() const {return Vector3 (0,0,0);}
    virtual Vector3 getBBMax() const {return Vector3 (0,0,0);}

    float getSignedDistance(Vector3 rayOrigin) const override {
        float d1 = prim->getSignedDistance(rayOrigin);
        float d2 = sin(displacement*rayOrigin[0])*sin(displacement*rayOrigin[1])*sin(displacement*rayOrigin[2]);
        return d1+d2;
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
    float displacement;
    Primitive* prim;
};

#endif
