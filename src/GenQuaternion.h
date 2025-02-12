#pragma once

#include <GenVector.h>

template<class T = float>
class GenQuaternion {
private:
    T a, i, j, k;
    
    GenQuaternion(T _a, T _i, T _j, T _k)
        : a(_a), i(_i), j(_j), k(_k) { }
    
public:
    
    // Identity
    GenQuaternion() : GenQuaternion(1, 0, 0, 0) { }
    
    // Euler -> GenQuaternion
    GenQuaternion(const GenVector<3, T> &euler)
        : GenQuaternion(euler[0], euler[1], euler[2]) { }
    
    GenQuaternion(const T x, const T y, const T z)
    {
        T hx = x / 2, hy = y / 2, hz = z / 2;
        T cu = cos(hx), su = sin(hx);
        T cv = cos(hy), sv = sin(hy);
        T cw = cos(hz), sw = sin(hz);
        a = cu * cv * cw + su * sv * sw;
        i = su * cv * cw - cu * sv * sw;
        j = cu * sv * cw + su * cv * sw;
        k = cu * cv * sw - su * sv * cw;
    }
    
    // Angle+Axis -> GenQuaternion
    GenQuaternion(const T theta, const GenVector<3, T> &axis)
    {
        T halfTheta = theta / 2;
        T sinHalfTheta = sin(halfTheta);
        a = cos(halfTheta);
        i = axis[0] * sinHalfTheta;
        j = axis[1] * sinHalfTheta;
        k = axis[2] * sinHalfTheta;
    }
    
    // GenQuaternion -> Angle+Axis
    T angle() const {
        return 2 * acos(a);
    }
    
    GenVector<3, T> axis() const {
        T halfTheta = acos(a);
        if (halfTheta == 0)
            return GenVector<3, T>(1, 0, 0); // Technically this can be any axis
        T sinHalfTheta = sin(halfTheta);
        return GenVector<3, T>(
            i / sinHalfTheta,
            j / sinHalfTheta,
            k / sinHalfTheta
        );
    }
    
    // Operations
    GenQuaternion operator-() const {
        return GenQuaternion(a, -i, -j, -k);
    }
    
    GenQuaternion operator*(const GenQuaternion &q) const {
        return GenQuaternion(
            a*q.a - i*q.i - j*q.j - k*q.k,
            a*q.i + i*q.a - j*q.k + k*q.j,
            a*q.j + i*q.k + j*q.a - k*q.i,
            a*q.k - i*q.j + j*q.i + k*q.a
        );
    }
    
    GenQuaternion& operator*=(const GenQuaternion &q) {
        return (*this) = (*this) * q;
    }
    
    // Passive rotation (rotate the coordinate system around the point)
    GenVector<3, T> operator*(const GenVector<3, T> &v) const {
        return ((*this) * GenQuaternion(0, v[0], v[1], v[2]) * -(*this)).axis();
    }
    
    GenVector<3, T>& operator*=(const GenVector<3, T> &v) {
        return v = (*this) * v;
    }
    
    // Active rotation (rotate the point around the coordinate system)
    friend GenVector<3, T> operator*(const GenVector<3, T> &v, const GenQuaternion &q) {
        return (-q * GenQuaternion(0, v[0], v[1], v[2]) * q).axis();
    }
    
    friend GenVector<3, T>& operator*=(const GenVector<3, T> &v, const GenQuaternion &q) {
        return v = v * q;
    }
    
    GenQuaternion& operator=(const GenQuaternion &q) {
        a = q.a;
        i = q.i;
        j = q.j;
        k = q.k;
        return *this;
    }
    
};

typedef GenQuaternion<> Quaternion;