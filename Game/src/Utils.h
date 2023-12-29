#pragma once
#include <string>
#include "Mesh.h"
#include "Entity.h"

namespace Utils {

	// load mesh -- old api --
	bool LoadFromObjectFile(std::string filename, Mesh& mesh, bool UV, bool Normal);
	// load mesh new
	bool LoadInstance(std::string filename, MeshInstance& mesh, Entity id);

}