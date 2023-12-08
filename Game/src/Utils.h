#pragma once
#include <string>
#include "Mesh.h"


namespace Utils {
	bool LoadFromObjectFile(std::string, Mesh& mesh, bool UV, bool Normal);
	

	template<typename T>
	T Lerp(T start, T stop, float t);

}