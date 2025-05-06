#include "Intersection.hpp"

Ray::Ray(const Vector3 &o, const Vector3 &d, int dep)
    : origin(o), direction(normalize(d)), depth(dep) {}

bool Ray::intersectTriangle(
                         const Vector3 &v0,
                         const Vector3 &v1,
                         const Vector3 &v2,
                         double &t, double &alpha, double &beta) const
{
    Vector3 N = cross(v1 - v0, v2 - v0);
    double NdotDir = dot(N, this->direction);
    if (fabs(NdotDir) < EPSILON) return false;

    double d = dot(N, v0);
    t = (d - dot(N, this->origin)) / NdotDir; // using plane and ray equations
    if (t < EPSILON) return false;

    Vector3 P = this->origin + this->direction * t;
    double fullArea = 0.5 * length(N);
    if (fullArea < EPSILON) return false;

    double area0 = 0.5 * length(cross(v1 - P, v2 - P));
    double area1 = 0.5 * length(cross(v2 - P, v0 - P));
    double area2 = 0.5 * length(cross(v0 - P, v1 - P));

    alpha = area0 / fullArea;
    beta  = area1 / fullArea;
    double gamma = area2 / fullArea;

    if (fabs(alpha + beta + gamma - 1.0) > 1e-3) return false;
    if (alpha < 0 || beta < 0 || gamma < 0) return false;

    return true;
}