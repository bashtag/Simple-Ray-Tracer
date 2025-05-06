#ifndef ILLUMINATION_HPP
#define ILLUMINATION_HPP

#include <cmath>
#include <map>
#include "Geometry.hpp"
#include "Intersection.hpp"
#include "Material.hpp"

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

class Scene;

class Illumination
{
public:
	// Constructor
	Illumination() {}
	// Destructor
	~Illumination() {}

	inline void setScene(const Scene* scene) { scene_ = scene; }
	inline void setAmbientLight(const Color& ambientLight) { ambientLight_ = ambientLight; }

	inline void copyMaterials(const map<string, Material>& materials) { materials_ = map<string, Material>(materials); }
	inline void copyPointLights(const vector<PointLight>& pointLights) { pointLights_ = vector<PointLight>(pointLights); }
	inline void copyTriangularLights(const vector<TriangularLight>& triangularLights) { triangularLights_ = vector<TriangularLight>(triangularLights); }

	// Function to calculate the illumination at a point
	Color calculateIlluminationPhongShading(const Hit& hit, const Vector3& viewDir) const;

private:
	const Scene* scene_;

	map<string, Material> materials_;
	Color ambientLight_;

	// Lights
	vector<PointLight> pointLights_;
	vector<TriangularLight> triangularLights_;

	Color pointLightPhongShading(const Hit& hit, const Vector3& viewDir, const Vector3& plPosition, const Color& intensity) const;

	// shadow test for lights
	bool isInShadow(const Hit& hit, const Vector3 &lightDir, double maxDist) const;
};




#endif// ILLUMINATION_HPP