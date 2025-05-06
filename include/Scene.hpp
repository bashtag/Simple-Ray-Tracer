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
#include "Geometry.hpp"
#include "Intersection.hpp"
#include "Illumination.hpp"

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

	// Camera
	Camera camera;
	Illumination illumination;

	// Materials, vertices, texture coordinates, normals, meshes
	map<string, Material> materials;
	double textureFactor;
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
