#include "Illumination.hpp"
#include "Scene.hpp"

using namespace std;

Color Illumination::pointLightPhongShading(const Hit& hit, const Vector3& viewDir, const Vector3& plPosition, const Color& intensity) const
{
	Vector3 L = plPosition - hit.position;
	double dist = length(L);
	L = normalize(L);

	if (isInShadow(hit, L, dist)) {
		return Color(0, 0, 0); // in shadow, no contribution
	}

	const Material &mat = this->materials_.at(hit.materialId);
	Color color(0,0,0);

	// diffuse
	double NdotL = max(0.0, dot(hit.normal, L));
	Color diffuse = mat.diffuse * intensity * NdotL * (1.0 / (dist * dist)); // to improve realism, we can use 1/(dist^2) for point light
	
	// specular
	Vector3 R = reflect(-L, hit.normal); // reflect the *light* vector
	double RdotV = max(0.0, dot(R, viewDir));
	Color spec = mat.specular * intensity * pow(RdotV, mat.phongExponent);

	color += diffuse + spec;

	return color;
}


bool Illumination::isInShadow(const Hit& hit, const Vector3 &lightDir, double maxDist) const
{
	// offset the origin a bit to avoid self-intersection
	Ray shadowRay(hit.position + lightDir * EPSILON, lightDir);
	Hit shadowHit;
	if ((*scene_).intersect(shadowRay, shadowHit))
	{
		// if the intersection is closer than the light distance, we are in shadow
		if (shadowHit.t < maxDist - EPSILON)
		{
			return true;
		}
	}
	return false;
}

Color Illumination::calculateIlluminationPhongShading(const Hit& hit, const Vector3& viewDir) const
{
	// get material
	const Material &mat = this->materials_.at(hit.materialId);
	Color color(0, 0, 0);

	// ambient
	color += mat.ambient * this->ambientLight_;

	// point lights
	for (auto &pl : this->pointLights_) {
		color += pointLightPhongShading(hit, viewDir, pl.position, pl.intensity);
	}

	// triangular lights
	for (auto &tl : this->triangularLights_) {
		color += pointLightPhongShading(hit, viewDir, tl.v1, tl.intensity);
		color += pointLightPhongShading(hit, viewDir, tl.v2, tl.intensity);
		color += pointLightPhongShading(hit, viewDir, tl.v3, tl.intensity);
	}

	return color;
}