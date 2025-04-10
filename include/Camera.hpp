#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Geometry.hpp"

class Camera {
public:
    Vector3 position;
    Vector3 gaze; // direction from camera to scene
    Vector3 up;
    double nearDistance;
    double left, right, bottom, top;
    int imWidth, imHeight;

    // Computed orthonormal basis vectors:
    Vector3 u, v, w;

    void computeCameraBasis();
};

#endif // CAMERA_HPP
