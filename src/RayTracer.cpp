#include "RayTracer.hpp"

using namespace std;

RayTracer::RayTracer(const Scene &scene): scene_(scene) {
	width_ = scene.camera.imWidth;
	height_ = scene.camera.imHeight;
	image_.resize(width_ * height_ * 4, 0); // RGBA
}

void RayTracer::saveImage(const std::string &filename) {
    unsigned error = lodepng::encode(filename, image_, width_, height_);
    if (error) {
        std::cerr << "PNG encoder error " << error << ": " 
                  << lodepng_error_text(error) << std::endl;
    } else {
        std::cout << "Rendering complete. Saved image to " << filename << std::endl;
    }
}

void RayTracer::render()
{
	Vector3 m = scene_.camera.position - scene_.camera.w * scene_.camera.nearDistance;
	Vector3 q = m + scene_.camera.u * scene_.camera.left + scene_.camera.v * scene_.camera.top;
	double r_minus_l = scene_.camera.right - scene_.camera.left;
	double t_minus_b = scene_.camera.top - scene_.camera.bottom;

	// each pixel
	for (int j = 0; j < height_; j++) {
        for (int i = 0; i < width_; i++) {
            double s_u = (r_minus_l) * ((i + 0.5) / (double)width_);
            double s_v = (t_minus_b) * ((j + 0.5) / (double)height_);
            
            Vector3 imagePoint = q + scene_.camera.u * s_u - scene_.camera.v * s_v;

            Ray ray(scene_.camera.position, imagePoint - scene_.camera.position);

			// result of tracing
            Color pixelColor = traceRay(ray, 0);
            
            int index = 4 * (j * width_ + i);
            image_[index + 0] = clamp8(pixelColor.r); 
            image_[index + 1] = clamp8(pixelColor.g);
            image_[index + 2] = clamp8(pixelColor.b);
            image_[index + 3] = 255; 
        }
        if (j % 50 == 0) {
            cout << "Rendered " << j << " / " << height_ << " rows." << endl;
        }
    }
}

void RayTracer::renderMultithreaded()
{
	Vector3 m = scene_.camera.position - scene_.camera.w * scene_.camera.nearDistance;
	Vector3 q = m + scene_.camera.u * scene_.camera.left + scene_.camera.v * scene_.camera.top;
	double r_minus_l = scene_.camera.right - scene_.camera.left;
	double t_minus_b = scene_.camera.top - scene_.camera.bottom;

	int numThreads = thread::hardware_concurrency();
	cout << "Using " << numThreads << " threads." << endl;
	if (numThreads == 0) {
		numThreads = 8;
	}
	vector<thread> threads;
	int rowsPerThread = height_ / numThreads; // each thread handles this many rows

	mutex printMutex; // mutex for cout

	for (int threadNumber = 0; threadNumber < numThreads; ++threadNumber)
	{
		int start = rowsPerThread * threadNumber;
		int end = rowsPerThread * (threadNumber + 1);

		// i wrote this because if don't do this, the last thread will not handle the last rows
		if (threadNumber == numThreads - 1) {
			end = height_; // last thread handles the rest
		}

		threads.emplace_back([this, start, end, r_minus_l, t_minus_b, q, &printMutex]() {
			{
				lock_guard<mutex> lock(printMutex);
				cout << "Rendering started between " << start << " and " << end << " rows." << endl;
			}
			for (int j = start; j < end; j++) {
				for (int i = 0; i < width_; i++) {
					double s_u = (r_minus_l) * ((i + 0.5) / static_cast<double>(width_));
					double s_v = (t_minus_b) * ((j + 0.5) / static_cast<double>(height_));
					
					Vector3 imagePoint = q + scene_.camera.u * s_u - scene_.camera.v * s_v;

					Ray ray(scene_.camera.position, imagePoint - scene_.camera.position);

					// result of tracing
					Color pixelColor = traceRay(ray, 0);
					
					int index = 4 * (j * width_ + i);
					image_[index + 0] = clamp8(pixelColor.r); 
					image_[index + 1] = clamp8(pixelColor.g);
					image_[index + 2] = clamp8(pixelColor.b);
					image_[index + 3] = 255; 
				}
			}
			{
				lock_guard<mutex> lock(printMutex);
				cout << "Rendering finished between " << start << " and " << end << " rows." << endl;
			}
		});
	}
	for (thread &thread: threads) {
		thread.join();
	}
}

Color RayTracer::traceRay(const Ray &ray, int depth) {
    if (depth > scene_.maxDepth) {
        return scene_.background;
    }

    Hit hit;
    if (!scene_.intersect(ray, hit)) {
        return scene_.background;
    }

    // local shading
    Vector3 viewDir = normalize(-ray.direction);

    Color localColor = scene_.illumination.calculateIlluminationPhongShading(hit, viewDir);

    // if there's a texture, blend it
    const Material &mat = scene_.materials.at(hit.materialId);
    if (!scene_.textureImage.empty() && mat.textureFactor > 0.0) {
        Color texColor = scene_.sampleTexture(hit.uv);
        // combine (1 - tf)*local + tf*texture
        localColor = localColor * (1.0 - mat.textureFactor) + texColor * mat.textureFactor;
    }

    // reflection
    if ((mat.mirror.r > EPSILON || mat.mirror.g > EPSILON || mat.mirror.b > EPSILON)) {
        Vector3 R = reflect(ray.direction, hit.normal);
        Ray reflectRay(hit.position + R*EPSILON, R, depth+1); // offset to avoid self-intersection
        Color reflectColor = traceRay(reflectRay, depth+1);
        localColor += reflectColor * mat.mirror;
    }

    return localColor;
}