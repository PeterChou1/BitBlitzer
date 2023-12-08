#pragma once
#include "Vec3.h"
#include <vector>
#include "Quat.h"
#include "Transform.h"


struct Vertex {
	Vec2 tex;
	Vec3 normal;
	Vec3 pos;
	float invW;

	Vertex() : invW(0) {}
	Vertex(Vec3& pos) : pos(pos), normal(Vec3(0, 0, 0)), tex(Vec2(0,0)), invW(0) {}
	Vertex(Vec3& pos, Vec2& tex) : pos(pos), tex(tex), invW(0) {}
	Vertex(Vec3& pos, Vec3& normal, Vec2& tex) : pos(pos), normal(normal), tex(tex), invW(0) {}
};

struct Triangle {

	Vertex verts[3];

	Triangle(const Triangle& t) {
		std::copy(std::begin(t.verts), std::end(t.verts), std::begin(verts));
	}

	Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3) : verts{ v1, v2, v3 } {}

	Triangle() : verts{} {}

	Triangle transform(const Transform& rhs);

};



struct Mesh {
	std::vector<Triangle> tris;
	// flag to tell if we should load normal or generate them automatically while rendering
	bool hasNormal;
	Mesh() : hasNormal(false) {};
};


