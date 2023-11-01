#include "stdafx.h"
#include "Utils.h"
#include <fstream>
#include <vector>
#include <strstream>

bool Utils::LoadFromObjectFile(std::string filename, Mesh& mesh)
{

	std::ifstream f(filename);

	if (!f.is_open())
		return false;

	// Local cache of verts
	std::vector<Vec3> verts;
	// Clear currently stored vert
	mesh.tris.clear();

	while (!f.eof())
	{
		char line[128];
		f.getline(line, 128);

		std::strstream s;
		s << line;

		char junk;

		if (line[0] == 'v')
		{
			Vec3 v;
			s >> junk >> v.x >> v.y >> v.z;
			verts.push_back(v);
		}

		if (line[0] == 'f')
		{
			int f[3];
			s >> junk >> f[0] >> f[1] >> f[2];

			mesh.tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
		}
	}
    return true;
}
