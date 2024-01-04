#pragma once
#include <string>
#include "Mesh.h"
#include "Entity.h"
#include "Texture.h"

namespace Utils
{
    // load .obj file
    bool LoadInstance(std::string filename, MeshInstance& mesh, std::vector<Texture>& textureList);

    // loader for an .mtl material file
    bool LoadMTLFile(const std::string& filename, std::vector<Texture>& textureList, std::unordered_map<std::string, size_t>& textureIDs);
}
