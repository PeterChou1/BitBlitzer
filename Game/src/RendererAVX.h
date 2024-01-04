#pragma once
#include <vector>
#include "Tile.h"
#include "Renderer.h"
#include "SIMDDepthBuffer.h"
#include "SIMDPixel.h"
#include "SIMDPixelBuffer.h"

/*
* /brief multi threaded renderer
*/
class RendererAVX : public Renderer
{
public:
    RendererAVX(int width, int height, Camera& cam);

    void Render() override;

    void VertexShaderStage();

    void ClippingStage();

    void TiledRasterizationStage();

    void RasterizationStage();

    void FragmentShaderStage();

    void UpdateFrameBuffer();

    void ClearBuffer();

private:
    std::vector<std::uint32_t> m_CoreIds;
    std::vector<std::vector<Triangle>> m_ProjectedClip;
    std::vector<Tile> m_Tiles;
    SIMDDepthBuffer m_DepthBuffer;
    SIMDPixelBuffer m_PixelBuffer;
    unsigned int m_CoreCount;
    int m_CoreInterval;
};
