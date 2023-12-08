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

	void RenderTriangle(Triangle& tri, SimpleTexture& tex);

	void DebugDraw(const Triangle& tri);

	void Render();

	void PainterSort(std::vector<Triangle>& mesh);

};
