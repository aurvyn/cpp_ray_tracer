#ifndef __TWISTER
#define __TWISTER

#include "Primitive.h"

class Twister : public Primitive {
public:
    Twister(Primitive* primitive, float twisty) : prim(primitive), twisty(twisty) {}

    virtual Vector3 getBBMin() const {
        LOG_WARN("Helix getBBMin is not implemented");
        return Vector3 (0,0,0);
    }
    virtual Vector3 getBBMax() const {
        LOG_WARN("Helix getBBMin is not implemented");
        return Vector3 (0,0,0);
    }

    float getSignedDistance(Vector3 rayOrigin) const override {
        float c = cos(twisty*rayOrigin[0]);
        float s = sin(twisty*rayOrigin[0]);
        float y_rot = c * rayOrigin[1] - s * rayOrigin[2];
        float z_rot = s * rayOrigin[1] + c * rayOrigin[2];
        return prim->getSignedDistance(Vector3(rayOrigin[0], y_rot, z_rot));
    }


    bool isSDF() const override { return true; };
private:
    float twisty;
    Primitive* prim;
};

#endif
