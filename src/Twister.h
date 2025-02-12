#ifndef __TWISTER
#define __TWISTER

#include "Primitive.h"

class Twister : public Primitive {
public:
    Twister(Primitive* primitive, float twisty) : prim(primitive), twisty(twisty) {}

    virtual Vector3 getBBMin() const {return Vector3 (0,0,0);}
    virtual Vector3 getBBMax() const {return Vector3 (0,0,0);}

    float getSignedDistance(Vector3 rayOrigin) const override {
        float c = cos(twisty*rayOrigin[0]);
        float s = sin(twisty*rayOrigin[0]);
        float y_rot = c * rayOrigin[1] - s * rayOrigin[2];
        float z_rot = s * rayOrigin[1] + c * rayOrigin[2];
        return prim->getSignedDistance(Vector3(rayOrigin[0], y_rot, z_rot));
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
    float twisty;
    Primitive* prim;
};

#endif
