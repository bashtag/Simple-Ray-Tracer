#ifndef SHADING_HPP
#define SHADING_HPP

#include "Scene.hpp"
#include "Intersection.hpp"

Color phongShading(const Scene &scene, const Hit &hit, const Vector3 &viewDir);

#endif // SHADING_HPP
