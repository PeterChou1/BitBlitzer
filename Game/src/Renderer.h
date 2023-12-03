#pragma once
#include <vector>
#include "Mesh.h"
#include "Mat4.h"
#include "System.h"

/*
* /brief CPU ECS
*/

class Renderer : public System {
public:
	Renderer() {};



	std::vector<Triangle> ClipTriangle(Vec3& planePoint, Vec3& planeNormal, Triangle& clip);

	std::vector<Triangle> Clip(std::vector<Triangle>& mesh);


	void DebugDraw(const Triangle& tri);

	void Render();

	void PainterSort(std::vector<Triangle>& mesh);

};

/*
* /brief Renderer based on OpenGL 1.1
*		 due to the limitations of OpenGL 1.1 
*		 GPU pipelining with vertex and fragment shaders are omitted
*/
class RendererGPU {

};