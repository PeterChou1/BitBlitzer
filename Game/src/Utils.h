#pragma once
#include <string>
#include "Mesh.h"
#include "Entity.h"
#include "SimpleTexture.h"

namespace Utils {

	// load mesh -- old api --
	bool LoadFromObjectFile(std::string filename, Mesh& mesh, bool UV, bool Normal);

	// load mesh new
    bool LoadInstance(std::string filename,
                     MeshInstance& mesh,
                     TextureList& texList);

	// load mtl mesh
    bool LoadMTLFile(const std::string& filename, TextureList& texList);

}
