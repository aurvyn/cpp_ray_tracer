//
// Created by agmui on 2/10/25.
//

#ifndef __TRACER_JULIASET
#define __TRACER_JULIASET

#include <cstring>

#include "GenVector.h"
#include "GLSLTypes.h"
#include "Ray.h"

using namespace GLSLTypes;

const int numIterations = 11;

/**
 * code obtained from https://iquilezles.org/articles/juliasets3d/
 */
class JuliaSet : public Primitive{
private:

//    vec4 cos(vec4 a) {
//      vec4 asincos = csincos(a.xy);
//      return vec4(asincos.zw, cmul(-asincos.xy, a.zw));
//    }
    float dot(vec4 a, vec4 b) const {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    float qlength2(vec4 q) const {
        return dot(q, q);
    }

    vec3 normalize(vec3 v) {
        float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        return vec3(v.x / len, v.y / len, v.z / len);
    }

    vec4 qsqr(vec4 a) const // square a quaterion
    {
        return vec4(a.x * a.x - a.y * a.y - a.z * a.z - a.w * a.w,
                    2.0 * a.x * a.y,
                    2.0 * a.x * a.z,
                    2.0 * a.x * a.w);
    }

    vec3 abs(vec3 a) const {
        return vec3(std::abs(a.x), std::abs(a.y), std::abs(a.z));
    }

    vec4 min(vec4 a, vec4 b) const {
        float len_a = qlength2(a);
        float len_b = qlength2(b);
        if (len_a < len_b)
            return a;
        return b;
    }

public:
    JuliaSet(vec4 c) : c(c) {}

    virtual Vector3 getBBMin() const {
        LOG_WARN("Julia Set getBBMin is not implemented");
        return {0,0,0};
    }
    virtual Vector3 getBBMax() const {
        LOG_WARN("Julia Set getBBMax is not implemented");
        return {0,0,0};
    }

    float map(vec3 p, vec4 &oTrap, vec4 c) const {
        vec4 z = vec4(p.x, p.y, p.z, 0.0);
        float md2 = 1.0;
        float mz2 = dot(z, z);

        auto z_abs = abs(vec3(z.x, z.y, z.z)); // auto aaaa = abs(z.xyz);
        vec4 trap = vec4(z_abs.x, z_abs.y, z_abs.z, dot(z, z));

        float n = 1.0;
        for (int i = 0; i < numIterations; i++) {
            // dz -> 2·z·dz, meaning |dz| -> 2·|z|·|dz|
            // Now we take the 2.0 out of the loop and do it at the end with an exp2
            md2 *= 4.0 * mz2;
            // z  -> z^2 + c
            z = qsqr(z) + c;

            auto z_abs = abs(vec3(z.x, z.y, z.z)); // auto aaaa = abs(z.xyz);
            trap = min(trap, vec4(z_abs.x, z_abs.y, z_abs.z, dot(z, z)));

            mz2 = qlength2(z);
            if (mz2 > 4.0) break;
            n += 1.0;
        }

        oTrap = trap;

        return 0.25 * sqrt(mz2 / md2) * log(mz2);  // d = 0.5·|z|·log|z|/|z'|
    }

    float intersect(const Ray &r, vec4 &res, vec4 c) {
        Vector3 ro = r.getOrigin();
        Vector3 rd = r.getDirection();
        vec4 tmp(0, 0, 0, 0);

        float resT = -1.0;
        float maxd = 10.0;
        float h = 1.0;
        float t = 0.0;
        for (int i = 0; i < 300; i++) {
            if (h < 0.0001 || t > maxd) break;
            Vector3 p = ro + rd * t;
            h = map(vec3(p[0], p[1], p[2]), tmp, c);
            t += h;
        }
        if (t < maxd) {
            resT = t;
            res = tmp;
        }

        return resT;
    }

    float getSignedDistance(Vector3 rayOrigin) const {
        vec4 oTrap(0,0,0,0);
        return map({rayOrigin[0], rayOrigin[1], rayOrigin[2]}, oTrap, c);
    }

    Vector3 getSDFNorm(Vector3 qo) const {
        vec4 q = vec4(qo[0], qo[1], qo[2], 0.0);
        mat4x4 J = identity_4x4();
        for (int i = 0; i < numIterations; i++) {
            J = J * mat4x4(q.x, -q.y, -q.z, -q.w,
                           q.y, q.x, 0.0, 0.0,
                           q.z, 0.0, q.x, 0.0,
                           q.w, 0.0, 0.0, q.x);
            q = qsqr(q) + c;
        }
        auto rez = (J * q);
        return Vector3(rez.x, rez.y, rez.z).normalize();
    }

    bool isSDF() const override { return true; };

private:
    vec4 c;

    /*
private:

    vec2 ccoj(vec2 a) {
        return vec2(a.x, -a.y);

    }

    vec4 csqr(vec4 a) {
        return vec4(
                csqr(a.xy),
                2.0 * cmul(a.xy, a.zw)
        );
    }

    vec4 cmul(vec4 a, vec2 b) {
        return vec4(
                cmul(a.xy, b.xy),
                cmul(a.zw, b.xy)
        );
    }

    vec4 qsqr(vec4 a) // square a quaterion
    {
        return vec4(a.x * a.x - a.y * a.y - a.z * a.z - a.w * a.w,
                    2.0 * a.x * a.y,
                    2.0 * a.x * a.z,
                    2.0 * a.x * a.w);
    }

    vec4 qmul(vec4 a, vec4 b) {
        return vec4(
                a.x * b.x - a.y * b.y - a.z * b.z - a.w * b.w,
                a.y * b.x + a.x * b.y + a.z * b.w - a.w * b.z,
                a.z * b.x + a.x * b.z + a.w * b.y - a.y * b.w,
                a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y);

    }

    vec4 qconj(vec4 a) {
//        return vec4(a.x, -a.yzw);
        return vec4(a.x, -a.y, -a.z, -a.w);
    }


public:

//    /*
    vec3 calcNormal(vec3 zo, vec4 c) {
        vec4 z = vec4(zo.x, zo.y, zo.z, 0.0);
        vec2 d = vec2(1.0, 0.0);
        for (int i = 0; i < numIterations; i++) {
            d = cmul(z, d);
            z = csqr(z) + c;
        }
        return normalize(cmul(z, cconj(dz)));
    }
//     */
    /*
    vec3 calcNormal( in vec3 q, in vec4 c )
    {
        const vec2 e = vec2(0.001,0.0);
        vec4 qa=vec4(q+e.xyy,0.0); float mq2a=qlength2(qa), md2a=1.0;
        vec4 qb=vec4(q-e.xyy,0.0); float mq2b=qlength2(qb), md2b=1.0;
        vec4 qc=vec4(q+e.yxy,0.0); float mq2c=qlength2(qc), md2c=1.0;
        vec4 qd=vec4(q-e.yxy,0.0); float mq2d=qlength2(qd), md2d=1.0;
        vec4 qe=vec4(q+e.yyx,0.0); float mq2e=qlength2(qe), md2e=1.0;
        vec4 qf=vec4(q-e.yyx,0.0); float mq2f=qlength2(qf), md2f=1.0;
        for(int i=0; i<numIterations; i++)
        {
            md2a *= mq2a; qa = qsqr(qa) + c; mq2a = qlength2(qa);
            md2b *= mq2b; qb = qsqr(qb) + c; mq2b = qlength2(qb);
            md2c *= mq2c; qc = qsqr(qc) + c; mq2c = qlength2(qc);
            md2d *= mq2d; qd = qsqr(qd) + c; mq2d = qlength2(qd);
            md2e *= mq2e; qe = qsqr(qe) + c; mq2e = qlength2(qe);
            md2f *= mq2f; qf = qsqr(qf) + c; mq2f = qlength2(qf);
        }
        float da = sqrt(mq2a/md2a)*log2(mq2a);
        float db = sqrt(mq2b/md2b)*log2(mq2b);
        float dc = sqrt(mq2c/md2c)*log2(mq2c);
        float dd = sqrt(mq2d/md2d)*log2(mq2d);
        float de = sqrt(mq2e/md2e)*log2(mq2e);
        float df = sqrt(mq2f/md2f)*log2(mq2f);

        return normalize( vec3(da-db,dc-dd,de-df) );
    }
     */

};

#endif //__TRACER_JULIASET
