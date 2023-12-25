#pragma once
#include <vector>
#include "Mesh.h"
#include "Mat4.h"
#include "System.h"
#include "SimpleTexture.h"
#include "DepthBuffer.h"
#include "GraphicsBuffer.h"
#include "Camera.h"

/*
* /brief multithreaded renderer
*/
class RendererM : public System {
public:
    RendererM(GraphicsBuffer& g, Camera& cam);

    void Render();

    void VertexShaderStage();

    void ClippingStage();

    void TiledRasterizationStage();

    void RasterizationStage();

    void FragmentShaderStage();

    void UpdateFrameBuffer();

    void ClearBuffer();



private:
    std::vector<std::uint32_t> coreIds;
    std::vector<Vertex>& m_vertexBuffer;
    std::vector<std::uint32_t>& m_indexBuffer;
    std::vector<Vertex>& m_projectedVertexBuffer;
    std::vector<std::vector<Triangle>>& m_projectedClip;
    std::vector<Tile>& m_tiles;
    Camera& m_cam;
    int m_coreCount;
    int m_coreInterval;
};