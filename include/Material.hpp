#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "Geometry.hpp"

class Material {
public:
    Color ambient;
    Color diffuse;
    Color specular;
    Color mirror; // Reflection factor
    double phongExponent;
    double textureFactor;
};

#endif // MATERIAL_HPP
