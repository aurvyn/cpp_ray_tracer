//
// Created by agmui on 2/11/25.
//

#ifndef TRACER_MANDELBULB_H
#define TRACER_MANDELBULB_H


#include "GenVector.h"

#include "Primitive.h"

class Mandelbulb : public Primitive {
public:
    Mandelbulb(float power) : power(power) {}

    virtual Vector3 getBBMin() const {return Vector3 (0,0,0);}
    virtual Vector3 getBBMax() const {return Vector3 (0,0,0);}

    float getSignedDistance(Vector3 rayOrigin) const override {
        Vector3 z = rayOrigin;
        float dr = 1;
        float r;
        for (int i = 0; i < 15; i++) {
            r = z.length();
            if (r > 2)
                break;

            float theta = acos(z[2] / r) * power;
            float phi = atan2(z[1], z[0]) * power;
            float zr = pow(r, power);
            dr = pow(r, power - 1) * power * dr + 1;

            z = zr * Vector3(sin(theta) * cos(phi), sin(phi) * sin(theta));
            z += rayOrigin;
        }

        return 0.5 * log(r) * r / dr;
    }

    bool isSDF() const override { return true; };

private:
    float power;
};


#endif //TRACER_MANDELBULB_H
