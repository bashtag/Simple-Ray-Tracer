#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "Geometry.hpp"

// Define PointLight
struct PointLight {
    Vector3 position;
    Color intensity;
};

// Define TriangularLight
struct TriangularLight {
    Vector3 v1, v2, v3;
    Color intensity;
    inline Vector3 direction() const { return normalize(cross(v2 - v1, v3 - v1)); }
};

#endif // LIGHT_HPP
