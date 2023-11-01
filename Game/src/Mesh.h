#pragma once
#include "Vec3.h"
#include <vector>

struct Triangle {
	Vec3 tri[3];
	Triangle(const Vec3& pt1, const Vec3& pt2, const Vec3& pt3) : tri{ pt1, pt2, pt3 } {}
	Triangle() {}
};


struct Mesh {
	std::vector<Triangle> tris;
};