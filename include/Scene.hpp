#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include "Camera.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Light.hpp"
#include "Geometry.hpp"
#include "Intersection.hpp"

#include "../lib/tinyxml2.h"
#include "../lib/lodepng.h"

using namespace std;

class Scene
{
public:
	Scene() {}
	Scene(const Scene &scene);

	// Global scene settings
	int maxDepth;
	Color background;
	Color ambientLight;

	// Camera
	Camera camera;

	// Lights
	vector<PointLight> pointLights;
	vector<TriangularLight> triangularLights;

	// Materials, vertices, texture coordinates, normals, meshes
	map<string, Material> materials;
	vector<Vector3> vertices;
	vector<Vector2> texcoords;
	vector<Vector3> normals;
	vector<Mesh> meshes;

	// Texture image and dimensions
	vector<unsigned char> textureImage;
	unsigned textureWidth, textureHeight;

	// Sample texture color at given UV coordinates
	Color sampleTexture(const Vector2 &uv) const;

	// Intersection function for ray tracing
	bool intersect(const Ray &ray, Hit &hit) const;

	// for illumination, phong shading is used
	Color phongShading(const Hit &hit, const Vector3 &viewDir) const;
	// shadow test for lights
	inline bool isInShadow(const Vector3 &point, const Vector3 &lightDir, double maxDist) const
	{
		// offset the origin a bit to avoid self-intersection
		Ray shadowRay(point + lightDir * EPSILON, lightDir);
		Hit shadowHit;
		if (intersect(shadowRay, shadowHit))
		{
			// if the intersection is closer than the light distance, we are in shadow
			if (shadowHit.t < maxDist - EPSILON)
			{
				return true;
			}
		}
		return false;
	}

	// Load scene from XML file
	void parseScene(const string &filename);

private:
	// parse utils
	static double parseDouble(const string &s);
	static Vector3 parseVector3(const string &s);
	static Vector2 parseVector2(const string &s);
	static Color parseColor(const string &s);
};

#endif // SCENE_HPP
