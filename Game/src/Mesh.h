#pragma once
#include "Vec3.h"
#include <vector>
#include "Quat.h"

struct Triangle {
	Vec3 tri[3];
	Triangle(const Vec3& pt1, const Vec3& pt2, const Vec3& pt3) : tri{ pt1, pt2, pt3 } {}
	Triangle() {}
	// const Triangle operator*(const Transform& rhs);
};


// struct Transform {
// 	Vec3 position;
// 	Quat rotation;
// };

struct Mesh {
	// Transform transform;
	std::vector<Triangle> tris;
};


