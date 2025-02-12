#ifndef __HELIX
#define __HELIX

#include "Primitive.h"
#include "AnthonyMath.h"

class Helix : public Primitive {
private:
    float dot(vec2 a, vec2 b) const {
        return a.x * b.x + a.y * b.y;
    }
public:
    Helix(float fr, float r1, float r2) : fr(fr), r1(r1), r2(r2) {}

    virtual Vector3 getBBMin() const {return Vector3 (0,0,0);}
    virtual Vector3 getBBMax() const {return Vector3 (0,0,0);}

    float getSignedDistance(Vector3 rayOrigin) const override {
        // from https://www.shadertoy.com/view/ftyBRd
        vec2 nline = vec2(fr, 6.283185*r1 );
        vec2 pline = vec2(nline.y, -nline.x);
        float repeat = nline.x*nline.y;
        vec2 pc = vec2(rayOrigin[0],r1*atan2(rayOrigin[1],rayOrigin[2]));
        vec2  pp = vec2(dot(pc,pline),
                        dot(pc,nline));
        pp.x = std::round(pp.x/repeat)*repeat;
        vec2 qc = (nline*pp.y+pline*pp.x)/dot(nline,nline);
        qc.y /= r1;
        vec3 q = vec3(qc.x, sin(qc.y)*r1, cos(qc.y)*r1 );
        return (vec3(rayOrigin[0], rayOrigin[1], rayOrigin[2])-q).length()-r2;
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

    bool isSDF() const override { return true; };
private:
    float fr;
    float r1;
    float r2;
};

#endif
