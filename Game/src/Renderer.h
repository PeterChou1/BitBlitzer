#pragma once
#include <vector>
#include "Mesh.h"
#include "Mat4.h"
#include "System.h"
#include "SimpleTexture.h"

/*
* /brief CPU ECS
*/

class Renderer : public System {
public:
	Renderer() {};

	std::vector<Triangle> ClipTriangle(Vec3& planePoint, Vec3& planeNormal, Triangle& clip);

	void RenderTriangle(
		float x1, float y1, float u1, float v1,
		float x2, float y2, float u2, float v2,
		float x3, float y3, float u3, float v3,
		SimpleTexture tex
	);

	void DebugDraw(const Triangle& tri);

	void Render();

	void PainterSort(std::vector<Triangle>& mesh);

};
