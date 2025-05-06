#ifndef RAYTRACER_HPP
#define RAYTRACER_HPP

#include <thread>
#include <mutex>
#include "Scene.hpp"

class RayTracer {
public:
    RayTracer(const Scene &scene);
    void render();
	void renderMultithreaded();
    void saveImage(const std::string &filename);

private:
    const Scene &scene_;
	int width_;
	int height_;
	std::vector<unsigned char> image_;  // Final RGBA image buffer.
    
	// it is recursive for reflection part
	Color traceRay(const Ray &ray, int depth);

	inline static unsigned char clamp8(double x) { 
		return (unsigned char)(std::max(0.0, std::min(255.0, x))); 
	};
};

#endif // RAYTRACER_HPP
