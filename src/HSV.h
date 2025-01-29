#pragma once 

#include <math.h>
#include "GenVector.h"

Vector3 hsv2rgb(Vector3 in)
{
    double      hh, p, q, t, ff;
    long        i;
    Vector3     out;

    if(in[1] <= 0.0) {       // < is bogus, just shuts up warnings
        float v = in[2] * 255;
        out = Vector3(v, v, v);
        return out;
    }
    hh = in[0];
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in[2] * (1.0 - in[1]);
    q = in[2] * (1.0 - (in[1] * ff));
    t = in[2] * (1.0 - (in[1] * (1.0 - ff)));

    switch(i) {
    case 0:
        out = Vector3(255 * in[2], 255 * t, 255 * p);
        break;
    case 1:
        out = Vector3(255 * q, 255 * in[2], 255 * p);
        break;
    case 2:
        out = Vector3(255 * p, 255 * in[2], 255 * t);
        break;

    case 3:
        out = Vector3(255 * p, 255 * q, 255 * in[2]);
        break;
    case 4:
        out = Vector3(255 * t, 255 * p, 255 * in[2]);
        break;
    case 5:
    default:
        out = Vector3(255 * in[2], 255 * p, 255 * q);        
        break;
    }
    return out;     
}

Vector3 rgbToHsv(Vector3 in) {
    float r = in[0];
    float g = in[1];
    float b = in[2];
    float h;
    float s;
    float v;

    // Normalize RGB values to [0, 1]
    r /= 255.0;
    g /= 255.0;
    b /= 255.0;

    float cmax = std::max(std::max(r, g), b);
    float cmin = std::min(std::min(r, g), b);
    float diff = cmax - cmin;

    // Calculate Hue
    if (diff == 0) {
        h = 0; 
    } else if (cmax == r) {
        h = 60 * ((g - b) / diff) + 360; 
    } else if (cmax == g) {
        h = 60 * ((b - r) / diff) + 120; 
    } else { // cmax == b
        h = 60 * ((r - g) / diff) + 240; 
    }
    h = std::fmod(h, 360); // Ensure h is within [0, 360]

    // Calculate Saturation
    if (cmax == 0) {
        s = 0;
    } else {
        s = (diff / cmax);
    }

    // Calculate Value
    v = cmax;
    return Vector3(h,s,v);
}

