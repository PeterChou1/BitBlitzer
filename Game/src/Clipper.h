//---------------------------------------------------------------------------------
// Clipper.h
//---------------------------------------------------------------------------------
// Multi-threaded Clipper that runs on all cores of the machine
// Clips the triangle and outputs the clipped Triangle to raster space
// See: https://chaosinmotion.com/2016/05/22/3d-clipping-in-homogeneous-coordinates/
//
#pragma once

#include <memory>

#include "Camera.h"
#include "ClippedTriangleBuffer.h"
#include "CubeMap.h"
#include "IndexBuffer.h"
#include "RenderConstants.h"
#include "VertexBuffer.h"


class Clipper
{
public:
    Clipper();

    void Clip();

private:
    std::shared_ptr<CubeMap> m_CubeMap;
    std::shared_ptr<RenderConstants> m_RenderConstants;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;
    std::shared_ptr<Camera> m_Cam;
    std::shared_ptr<VertexBuffer> m_VertexBuffer;
    std::shared_ptr<ClippedTriangleBuffer> m_ClippedTriangleBuffer;

};
