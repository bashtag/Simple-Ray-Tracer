#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include "Geometry.hpp"
#include <string>
#include "Mesh.hpp"

class Ray {
public:
    Vector3 origin;
    Vector3 direction;
    int depth;

    Ray(const Vector3 &o, const Vector3 &d, int dep = 0);

    bool intersectTriangle(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2,
                             double &t, double &alpha, double &beta) const;
};

struct Hit {
    bool hit;
    double t;
    Vector3 position;
    Vector3 normal;
    std::string materialId;
    Vector2 uv;

    // barycentric coordinates
    double alpha, beta, gamma;
    Hit() : hit(false), t(std::numeric_limits<double>::max()) {};
};


#endif // INTERSECTION_HPP
