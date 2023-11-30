#pragma once
#include <string>
#include "Mesh.h"
#define _SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING

namespace Utils {
	bool LoadFromObjectFile(std::string, Mesh& mesh);
}