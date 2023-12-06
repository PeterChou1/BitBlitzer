#pragma once
#include "Vec3.h"
#include <vector>
#include "Quat.h"
#include "Transform.h"


struct Vertex {
	Vec2 tex;
	Vec3 normal;
	Vec3 pos;
	Vertex() {}
	Vertex(Vec3& pos) : pos(pos), normal(Vec3(0, 0, 0)), tex(Vec2(0,0)) {}
};

struct Triangle {

	Vertex vert[3];

	Vec2 tex[3];
	Vec3 tri[3];
	Vec3 normal;
	float r, g, b;

	Triangle(const Triangle& t) : r(t.r), g(t.g), b(t.b), normal(t.normal) {
		std::copy(std::begin(t.tri), std::end(t.tri), std::begin(tri));
		std::copy(std::begin(t.tex), std::end(t.tex), std::begin(tex));
	}
	Triangle(const Vec3& pt1, const Vec3& pt2, const Vec3& pt3) : tri{ pt1, pt2, pt3 }, tex{}, normal {}, r(1), g(1), b(1) {}
	Triangle(const Vec3& pt1, const Vec3& pt2, const Vec3& pt3, 
		     const Vec2& tex1, const Vec2& tex2, const Vec2& tex3) : tri{pt1, pt2, pt3}, tex{tex1, tex2, tex3}, normal {}, r(1), g(1), b(1) {}
	Triangle() : tri{}, tex{}, normal {}, r(1), g(1), b(1) {}

	const Triangle transform(const Transform& rhs);
};



struct Mesh {
	std::vector<Triangle> tris;
	// flag to tell if we should load normal or generate them automatically while rendering
	bool hasNormal;
	Mesh() : hasNormal(false) {};
};


