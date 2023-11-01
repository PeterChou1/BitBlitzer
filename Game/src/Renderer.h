#pragma once
#include <vector>
#include "Mesh.h"
#include "Mat4.h"



/*
* /brief CPU based Renderer
*/
class RendererCPU {
public:
	RendererCPU() {};
	void Init();
	void Render();
private:
	std::vector<Mesh> meshes;
	Mat4 proj;
	void PainterSort(std::vector<Triangle>& mesh);
};

/*
* /brief Renderer based on OpenGL 1.1
*		 due to the limitations of OpenGL 1.1 
*		 GPU pipelining with vertex and fragment shaders are omitted
*/
class RendererGPU {

};