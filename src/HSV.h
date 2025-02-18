#pragma once 

#include <math.h>
#include "GenVector.h"

/*
RGB values are in range [0,1]                          (when normalized)
HSV values are in range [H: [0,360], S: [0,1], V: [0,1]] (when normalized)
*/

Vector3 clampHSV(Vector3 hsv) {
    return Vector3(clamp(0, 360.0f, hsv[0]), clamp(0, 1, hsv[1]), clamp(0, 1, hsv[2]));
}

Vector3 clampRGB(Vector3 rgb) {
    return Vector3(clamp(0, 1, rgb[0]), clamp(0, 1, rgb[1]), clamp(0, 1, rgb[2]));
}

Vector4 clampHSV(Vector4 hsv) {
    return Vector4(clamp(0, 360.0f, hsv[0]), clamp(0, 1, hsv[1]), clamp(0, 1, hsv[2]), clamp(0, 1, hsv[3]));
}

Vector4 clampRGB(Vector4 rgb) {
    return Vector4(clamp(0, 1, rgb[0]), clamp(0, 1, rgb[1]), clamp(0, 1, rgb[2]), clamp(0, 1, rgb[3]));
}



int hexVal(char hexVal) {
    if(hexVal >= 'a') return hexVal - 'a' + 10;
    else if(hexVal >= 'A') return hexVal - 'A' + 10;
    else if(hexVal >= '0') return hexVal - '0';
    else return 0;
}

Vector3 rgbFromHex(std::string hexStr) {
    const char* hex = hexStr.c_str();
    int r = hexVal(hex[1]) + 16 * hexVal(hex[0]);
    int g = hexVal(hex[3]) + 16 * hexVal(hex[2]);
    int b = hexVal(hex[5]) + 16 * hexVal(hex[4]);
    return Vector3((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f);
}






Vector3 hsv2rgb(Vector3 in)
{
    double      hh, p, q, t, ff;
    long        i;
    Vector3     out;

    if(in[1] <= 0.0) {       // < is bogus, just shuts up warnings
        float v = in[2];
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
        out = Vector3(in[2], t, p);
        break;
    case 1:
        out = Vector3(q, in[2], p);
        break;
    case 2:
        out = Vector3(p, in[2], t);
        break;

    case 3:
        out = Vector3(p, q, in[2]);
        break;
    case 4:
        out = Vector3(t, p, in[2]);
        break;
    case 5:
    default:
        out = Vector3(in[2], p, q);        
        break;
    }
    return out;     
}

Vector3 rgb2hsv(Vector3 in) {
    float r = in[0];
    float g = in[1];
    float b = in[2];
    float h;
    float s;
    float v;

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

Vector3 hsvFromHex(std::string hexStr) {
    return rgb2hsv(rgbFromHex(hexStr));
}
