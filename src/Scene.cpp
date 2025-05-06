#include "Scene.hpp"

using namespace std;
using namespace tinyxml2;

Scene::Scene(const Scene& scene)
{
	this->maxDepth = scene.maxDepth;
	this->background = scene.background;
	this->camera = scene.camera;
	this->materials = scene.materials;
	this->vertices = scene.vertices;
	this->texcoords = scene.texcoords;
	this->normals = scene.normals;
	this->meshes = scene.meshes;
	this->textureImage = scene.textureImage;
	this->textureWidth = scene.textureWidth;
	this->textureHeight = scene.textureHeight;
}

bool Scene::intersect(const Ray &ray, Hit &hit) const
{
	bool anyHit = false;
	for (const auto &mesh : this->meshes)
	{

		for (int fIdx = 0; fIdx < (int)mesh.faces.size(); fIdx++)
		{
			const Face &face = mesh.faces[fIdx];
			const Vector3 &v0 = this->vertices[face.v[0]];
			const Vector3 &v1 = this->vertices[face.v[1]];
			const Vector3 &v2 = this->vertices[face.v[2]];

			double t, alpha, beta;
			if (ray.intersectTriangle(v0, v1, v2, t, alpha, beta))
			{
				if (t < hit.t)
				{
					hit.hit = true;
					hit.t = t;
					hit.materialId = mesh.materialId;
					hit.position = ray.origin + ray.direction * t;

					const Vector3 &n0 = this->normals[face.n[0]];
					const Vector3 &n1 = this->normals[face.n[1]];
					const Vector3 &n2 = this->normals[face.n[2]];
					double gamma = 1.0 - alpha - beta;
					Vector3 N = n0 * gamma + n1 * alpha + n2 * beta;
					hit.normal = normalize(N);

					if (!this->texcoords.empty())
					{
						Vector2 uv0 = this->texcoords[face.t[0]];
						Vector2 uv1 = this->texcoords[face.t[1]];
						Vector2 uv2 = this->texcoords[face.t[2]];
						hit.uv.u = uv0.u * gamma + uv1.u * alpha + uv2.u * beta;
						hit.uv.v = uv0.v * gamma + uv1.v * alpha + uv2.v * beta;
					}
				}
				anyHit = true;
			}
		}
	}
	return anyHit;
}

void Scene::parseScene(const std::string &filename)
{
	XMLDocument doc;
	if (doc.LoadFile(filename.c_str()) != XML_SUCCESS)
	{
		cerr << "Error loading scene file " << filename << endl;
		exit(1);
	}
	XMLElement *root = doc.FirstChildElement("scene");
	if (!root)
	{
		cerr << "No <scene> element found" << endl;
		exit(1);
	}

	// maxraytracedepth
	XMLElement *maxDepthElem = root->FirstChildElement("maxraytracedepth");
	if (maxDepthElem && maxDepthElem->GetText())
		this->maxDepth = atoi(maxDepthElem->GetText());
	else
		this->maxDepth = 5;

	// background color
	XMLElement *bgElem = root->FirstChildElement("background");
	if (!bgElem)
	{
		bgElem = root->FirstChildElement("backgroundColor");
	}
	if (bgElem && bgElem->GetText())
		this->background = parseColor(bgElem->GetText());
	else
		this->background = Color(0, 0, 0);

	// Camera
	XMLElement *cameraElem = root->FirstChildElement("camera");
	if (cameraElem)
	{
		XMLElement *posElem = cameraElem->FirstChildElement("position");
		if (posElem && posElem->GetText())
			this->camera.position = parseVector3(posElem->GetText());

		XMLElement *gazeElem = cameraElem->FirstChildElement("gaze");
		if (gazeElem && gazeElem->GetText())
			this->camera.gaze = parseVector3(gazeElem->GetText());

		XMLElement *upElem = cameraElem->FirstChildElement("up");
		if (upElem && upElem->GetText())
			this->camera.up = parseVector3(upElem->GetText());

		XMLElement *npElem = cameraElem->FirstChildElement("nearPlane");
		if (npElem && npElem->GetText())
		{
			istringstream iss(npElem->GetText());
			iss >> this->camera.left >> this->camera.right >> this->camera.bottom >> this->camera.top;
		}

		XMLElement *ndElem = cameraElem->FirstChildElement("neardistance");
		if (ndElem && ndElem->GetText())
			this->camera.nearDistance = parseDouble(ndElem->GetText());

		XMLElement *resElem = cameraElem->FirstChildElement("imageresolution");
		if (resElem && resElem->GetText())
		{
			istringstream iss(resElem->GetText());
			iss >> this->camera.imWidth >> this->camera.imHeight;
		}
		this->camera.computeCameraBasis();
	}

	Color ambientLight;
	vector<PointLight> pointLights;
	vector<TriangularLight> triangularLights;

	// Lights
	XMLElement *lightsElem = root->FirstChildElement("lights");
	if (lightsElem)
	{
		// ambientlight
		XMLElement *ambElem = lightsElem->FirstChildElement("ambientlight");
		if (ambElem && ambElem->GetText())
			ambientLight = parseColor(ambElem->GetText());
		else
			ambientLight = Color(0, 0, 0);

		// pointlights
		for (XMLElement *plElem = lightsElem->FirstChildElement("pointlight");
			 plElem;
			 plElem = plElem->NextSiblingElement("pointlight"))
		{
			PointLight pl;
			XMLElement *posElem = plElem->FirstChildElement("position");
			if (posElem && posElem->GetText())
				pl.position = parseVector3(posElem->GetText());
			XMLElement *intenElem = plElem->FirstChildElement("intensity");
			if (intenElem && intenElem->GetText())
				pl.intensity = parseColor(intenElem->GetText());
			pointLights.push_back(pl);
		}
		// triangularlights
		for (XMLElement *tlElem = lightsElem->FirstChildElement("triangularlight");
			 tlElem;
			 tlElem = tlElem->NextSiblingElement("triangularlight"))
		{
			TriangularLight tl;
			XMLElement *v1Elem = tlElem->FirstChildElement("vertex1");
			XMLElement *v2Elem = tlElem->FirstChildElement("vertex2");
			XMLElement *v3Elem = tlElem->FirstChildElement("vertex3");
			if (v1Elem && v1Elem->GetText() &&
				v2Elem && v2Elem->GetText() &&
				v3Elem && v3Elem->GetText())
			{
				tl.v1 = parseVector3(v1Elem->GetText());
				tl.v2 = parseVector3(v2Elem->GetText());
				tl.v3 = parseVector3(v3Elem->GetText());
			}
			XMLElement *intenElem = tlElem->FirstChildElement("intensity");
			if (intenElem && intenElem->GetText())
				tl.intensity = parseColor(intenElem->GetText());
			triangularLights.push_back(tl);
		}
	}

	// Materials
	XMLElement *materialsElem = root->FirstChildElement("materials");
	if (materialsElem)
	{
		for (XMLElement *matElem = materialsElem->FirstChildElement("material");
			 matElem;
			 matElem = matElem->NextSiblingElement("material"))
		{
			const char *id = matElem->Attribute("id");
			if (!id)
				continue;
			Material m;
			XMLElement *ambElem = matElem->FirstChildElement("ambient");
			if (ambElem && ambElem->GetText())
				m.ambient = parseColor(ambElem->GetText());
			XMLElement *diffElem = matElem->FirstChildElement("diffuse");
			if (diffElem && diffElem->GetText())
				m.diffuse = parseColor(diffElem->GetText());
			XMLElement *specElem = matElem->FirstChildElement("specular");
			if (specElem && specElem->GetText())
				m.specular = parseColor(specElem->GetText());
			XMLElement *mirrorElem = matElem->FirstChildElement("mirrorreflactance");
			if (mirrorElem && mirrorElem->GetText())
				m.mirror = parseColor(mirrorElem->GetText());
			XMLElement *phongElem = matElem->FirstChildElement("phongexponent");
			if (phongElem && phongElem->GetText())
				m.phongExponent = parseDouble(phongElem->GetText());
			XMLElement *texElem = matElem->FirstChildElement("texturefactor");
			if (texElem && texElem->GetText())
			{
				m.textureFactor = parseDouble(texElem->GetText());
				this->textureFactor = m.textureFactor;
			}
			this->materials[id] = m;
		}
	}

	illumination.setAmbientLight(ambientLight);
	illumination.copyPointLights(pointLights);
	illumination.copyTriangularLights(triangularLights);
	illumination.copyMaterials(this->materials);
	illumination.setScene(this);

	// Vertex data
	XMLElement *vdataElem = root->FirstChildElement("vertexdata");
	if (vdataElem && vdataElem->GetText())
	{
		istringstream iss(vdataElem->GetText());
		double x, y, z;
		while (iss >> x >> y >> z)
		{
			this->vertices.push_back(Vector3(x, y, z));
		}
	}

	// Texture data
	XMLElement *tdataElem = root->FirstChildElement("texturedata");
	if (tdataElem && tdataElem->GetText())
	{
		istringstream iss(tdataElem->GetText());
		double u, v;
		while (iss >> u >> v)
		{
			this->texcoords.push_back(Vector2(u, v));
		}
	}

	// Texture image file name
	XMLElement *timgElem = root->FirstChildElement("textureimage");
	string textureFile;
	if (timgElem && timgElem->GetText())
		textureFile = timgElem->GetText();

	// Load texture image (if available)
	if (!textureFile.empty())
	{
		unsigned error = lodepng::decode(this->textureImage, this->textureWidth, this->textureHeight, textureFile);
		if (error)
		{
			cerr << "Error loading texture image " << textureFile << ": " << lodepng_error_text(error) << endl;
			// Continue without texture
			this->textureImage.clear();
			this->textureWidth = this->textureHeight = 0;
		}
	}

	// Normal data
	XMLElement *ndataElem = root->FirstChildElement("normaldata");
	if (ndataElem && ndataElem->GetText())
	{
		istringstream iss(ndataElem->GetText());
		double x, y, z;
		while (iss >> x >> y >> z)
		{
			this->normals.push_back(Vector3(x, y, z));
		}
	}

	// Objects (meshes)
	XMLElement *objectsElem = root->FirstChildElement("objects");
	if (objectsElem)
	{
		for (XMLElement *meshElem = objectsElem->FirstChildElement("mesh");
			 meshElem;
			 meshElem = meshElem->NextSiblingElement("mesh"))
		{
			Mesh mesh;
			XMLElement *matIdElem = meshElem->FirstChildElement("materialid");
			if (matIdElem && matIdElem->GetText())
				mesh.materialId = matIdElem->GetText();
			XMLElement *facesElem = meshElem->FirstChildElement("faces");
			if (facesElem && facesElem->GetText())
			{
				istringstream iss(facesElem->GetText());
				string token;
				while (iss >> token)
				{
					// Each face has 3 sets of (v/t/n)
					Face face;
					for (int i = 0; i < 3; i++)
					{
						size_t pos1 = token.find('/');
						size_t pos2 = token.find('/', pos1 + 1);
						face.v[i] = stoi(token.substr(0, pos1)) - 1;
						face.t[i] = stoi(token.substr(pos1 + 1, pos2 - pos1 - 1)) - 1;
						face.n[i] = stoi(token.substr(pos2 + 1)) - 1;
						if (i < 2)
						{
							iss >> token;
						}
					}
					mesh.faces.push_back(face);
				}
			}
			this->meshes.push_back(mesh);
		}
	}
}

double Scene::parseDouble(const string &s)
{
	return atof(s.c_str());
}

Vector3 Scene::parseVector3(const string &s)
{
	istringstream iss(s);
	double x, y, z;
	iss >> x >> y >> z;
	return Vector3(x, y, z);
}

Vector2 Scene::parseVector2(const string &s)
{
	istringstream iss(s);
	double u, v;
	iss >> u >> v;
	return Vector2(u, v);
}

Color Scene::parseColor(const string &s)
{
	istringstream iss(s);
	double r, g, b;
	iss >> r >> g >> b;
	return Color(r, g, b);
}

Color Scene::sampleTexture(const Vector2 &uv) const {
    if (this->textureImage.empty()) {
        // no texture
        return background;
    }

    // Wrap or clamp uv in [0,1]
	// because uv can be a little greater than 1.0 or less than 0.0 . for ex. 1.00018
    double u = fmod(uv.u, 1.0);
    double v = fmod(uv.v, 1.0);
    if (u < 0) u += 1.0;
    if (v < 0) v += 1.0;

    // compute pixel coords
    unsigned x = (unsigned)(u * (this->textureWidth - 1));
    // invert v for typical image coordinates
    unsigned y = (unsigned)((1.0 - v) * (this->textureHeight - 1));

    unsigned idx = 4 * (y * this->textureWidth + x);
    double r = this->textureImage[idx + 0];
    double g = this->textureImage[idx + 1];
    double b = this->textureImage[idx + 2];
    return Color(r, g, b);
}
