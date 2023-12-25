#pragma once
#include <vector>
#include "Mesh.h"
#include "Mat4.h"
#include "System.h"
#include "SimpleTexture.h"
#include "DepthBuffer.h"
#include "Camera.h"

/*
* /brief CPU ECS
*/

class Renderer : public System {
public:

	Renderer(Camera& cam, DepthBuffer& depth) : m_cam(cam), m_depth(depth) {};

	void RenderTriangle(Triangle& tri, SimpleTexture& tex, DepthBuffer& depth);

	void DebugDraw(const Triangle& tri);

	void Render();

private:
	Camera& m_cam;
	DepthBuffer& m_depth;
};
