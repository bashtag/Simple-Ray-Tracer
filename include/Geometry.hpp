#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <cmath>
#include "utils.hpp"

class Vector3 {
public:
    double x, y, z;
    Vector3(double xx = 0, double yy = 0, double zz = 0) : x(xx), y(yy), z(zz) {}

    Vector3 operator+(const Vector3 &v) const { return Vector3(x+v.x, y+v.y, z+v.z); }
    Vector3 operator-(const Vector3 &v) const { return Vector3(x-v.x, y-v.y, z-v.z); }
    Vector3 operator*(double s)     const { return Vector3(x*s, y*s, z*s); }
    Vector3 operator/(double s)     const { return Vector3(x/s, y/s, z/s); }
    Vector3 operator-() const { return Vector3(-x, -y, -z); }

};

class Vector2 {
public:
    double u, v;
    Vector2(double uu = 0, double vv = 0) : u(uu), v(vv) {}
};

class Color {
public:
    double r, g, b;
    Color(double rr = 0, double gg = 0, double bb = 0) : r(rr), g(gg), b(bb) {}
    Color operator+(const Color &c) const { return Color(r+c.r, g+c.g, b+c.b); }
    Color operator*(double s) const { return Color(r*s, g*s, b*s); }
    Color operator*(const Color &c) const { return Color(r*c.r, g*c.g, b*c.b); }
    Color& operator+=(const Color &c) { r += c.r; g += c.g; b += c.b; return *this; }
};

inline static double dot(const Vector3 &a, const Vector3 &b) { return a.x*b.x + a.y*b.y + a.z*b.z; }

inline static Vector3 cross(const Vector3 &a, const Vector3 &b) {
    return Vector3(
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    );
}

inline static double length(const Vector3 &v) { return sqrt(dot(v,v)); }

inline static Vector3 normalize(const Vector3 &v) {
    double len = length(v);
    if(len < EPSILON) return v;
    return v / len;
}

inline static Vector3 reflect(const Vector3 &I, const Vector3 &N) {
    // I is the incident direction (pointing *from* intersection *toward* the camera),
    // N is the surface normal (pointing outward).
    // Reflection is: R = I - 2 (I·N) N
    return I - N * (2.0 * dot(I, N));
}

#endif // GEOMETRY_HPP