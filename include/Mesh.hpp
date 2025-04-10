#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <string>
#include "Geometry.hpp"

using namespace std;

// A simple Face struct
struct Face {
    int v[3]; // vertex indices
    int t[3]; // texture coordinates indices
    int n[3]; // normal indices
};

class Mesh {
public:
    string materialId;
    vector<Face> faces;
};

#endif // MESH_HPP
