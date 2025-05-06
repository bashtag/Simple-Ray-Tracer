#include "Camera.hpp"

void Camera::computeCameraBasis()
{
	w = normalize(gaze * -1.0);
	u = normalize(cross(up, w));
	v = cross(w, u);
}