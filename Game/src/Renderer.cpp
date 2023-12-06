#include "stdafx.h"
#include <iostream>
#include <algorithm>
#include "../App/AppSettings.h"
#include "../app/app.h"
#include "Quat.h"
#include "Renderer.h"
#include "Utils.h"
#include "Coordinator.h"
#include "Camera.h"



extern Coordinator gCoordinator;

constexpr double ASPECT_RATIO = APP_VIRTUAL_WIDTH / APP_VIRTUAL_HEIGHT;


std::vector<Triangle> Renderer::ClipTriangle(Vec3& planePoint, Vec3& planeNormal, Triangle& clip)
{

    Vec3* insidePts[3]; Vec2* insideTex[3]; int insidePtCount = 0;
    Vec3* outsidePts[3]; Vec2* outsideTex[3]; int outsidePtCount = 0;

    std::vector<Triangle> clippedTriangle;


    float d0 = Dist(clip.verts[0].pos, planeNormal, planePoint);
    float d1 = Dist(clip.verts[1].pos, planeNormal, planePoint);
    float d2 = Dist(clip.verts[2].pos, planeNormal, planePoint);

    
    if (d0 >= 0) 
    { 
        insidePts[insidePtCount] = &clip.verts[0].pos;
        insideTex[insidePtCount]  = &clip.verts[0].tex;
        insidePtCount++;
    }
    else 
    { 
        outsidePts[outsidePtCount] = &clip.verts[0].pos;
        outsideTex[outsidePtCount] = &clip.verts[0].tex;
        outsidePtCount++;
    };

    if (d1 >= 0) 
    { 
        insidePts[insidePtCount] = &clip.verts[1].pos;
        insideTex[insidePtCount] = &clip.verts[1].tex;
        insidePtCount++;
    }
    else 
    { 
        outsidePts[outsidePtCount] = &clip.verts[1].pos;
        outsideTex[outsidePtCount] = &clip.verts[1].tex;
        outsidePtCount++;
    };

    if (d2 >= 0) 
    { 
        insidePts[insidePtCount] = &clip.verts[2].pos;
        insideTex[insidePtCount] = &clip.verts[2].tex;
        insidePtCount++;
    }
    else 
    { 
        outsidePts[outsidePtCount] = &clip.verts[2].pos;
        outsideTex[outsidePtCount] = &clip.verts[2].tex;
        outsidePtCount++;
    };


    if (insidePtCount == 0) 
    {
        // no triangle inside
        return clippedTriangle;
    }

    if (insidePtCount == 3) 
    {
        clippedTriangle.emplace_back(clip);
        return clippedTriangle;
    }

    if (insidePtCount == 1 && outsidePtCount == 2) 
    {
        Triangle newTri = clip;

        float t = 0;
        newTri.verts[0].pos = *insidePts[0];
        newTri.verts[0].tex = *insideTex[0];

        newTri.verts[1].pos = IntersectPlane(planePoint, planeNormal, newTri.verts[0].pos, *outsidePts[0], t);
        newTri.verts[1].tex = *insideTex[0] + (*outsideTex[0] - *insideTex[0]) * t;

        newTri.verts[2].pos = IntersectPlane(planePoint, planeNormal, newTri.verts[0].pos, *outsidePts[1], t);
        newTri.verts[2].tex = *insideTex[0] + (*outsideTex[1] - *insideTex[0]) * t;

        clippedTriangle.emplace_back(newTri);

        return clippedTriangle;
    }

    if (insidePtCount == 2 && outsidePtCount == 1)
    {
        Triangle tri1 = clip, tri2 = clip;
        float t1 = 0, t2 = 0;
        tri1.verts[0].pos = *insidePts[0];
        tri1.verts[0].tex = *insideTex[0];

        tri1.verts[1].pos = *insidePts[1];
        tri1.verts[1].tex = *insideTex[1];

        tri1.verts[2].pos = IntersectPlane(planePoint, planeNormal, *insidePts[0], *outsidePts[0], t1);
        tri1.verts[2].tex = *insideTex[0] + (*outsideTex[0] - *insideTex[0]) * t1;

        tri2.verts[0].pos = *insidePts[1];
        tri2.verts[0].tex = *insideTex[1];

        tri2.verts[1].pos = tri1.verts[2].pos;
        tri2.verts[1].tex = tri1.verts[2].tex;

        tri2.verts[2].pos = IntersectPlane(planePoint, planeNormal, *insidePts[1], *outsidePts[0], t2);
        tri2.verts[2].tex = *insideTex[1] + (*outsideTex[0] - *insideTex[1]) * t2;

        clippedTriangle.emplace_back(tri1);
        clippedTriangle.emplace_back(tri2);
        return clippedTriangle;
    }

    assert(false, "unreachable code");
}




void Renderer::RenderTriangle(
    float x1, float y1, float u1, float v1, 
    float x2, float y2, float u2, float v2, 
    float x3, float y3, float u3, float v3,
    SimpleTexture tex
)
{
    if (y2 < y1)
    {
        std::swap(y1, y2);
        std::swap(x1, x2);
        std::swap(u1, u2);
        std::swap(v1, v2);

    }

    if (y3 < y1)
    {
        std::swap(y1, y3);
        std::swap(x1, x3);
        std::swap(u1, u3);
        std::swap(v1, v3);

    }

    if (y3 < y2)
    {
        std::swap(y2, y3);
        std::swap(x2, x3);
        std::swap(u2, u3);
        std::swap(v2, v3);
    }


    auto FillBottom = [](
        float vx1, float vy1, float u1, float v1,
        float vx2, float vy2, float u2, float v2,
        float vx3, float vy3, float u3, float v3,
        SimpleTexture& tex
    ) {
        float delta = 1;
        float invslope1 = (vx2 - vx1) / (vy2 - vy1);
        float invslope2 = (vx3 - vx1) / (vy3 - vy1);

        invslope1 *= delta;
        invslope2 *= delta;

        float curx1 = vx1;
        float curx2 = vx1;


        if (invslope1 > invslope2) {
            std::swap(invslope1, invslope2);
        }


        for (float scanlineY = vy1; scanlineY <= vy2; scanlineY += delta)
        {
            for (float x = curx1; x < curx2; x += delta) {
                // Linearly interpolate u and v for the current pixel
                // float alpha;
                float det = (vy2 - vy3) * (vx1 - vx3) + (vx3 - vx2) * (vy1 - vy3);
                assert(abs(det) > std::numeric_limits<float>::epsilon(), "not possible");
                float u = ((vy2 - vy3) * (x - vx3) + (vx3 - vx2) * (scanlineY - vy3)) / det;
                float v = ((vy3 - vy1) * (x - vx3) + (vx1 - vx3) * (scanlineY - vy3)) / det;
                float w = 1 - u - v;
                float trueU = u * u1 + v * u2 + w * u3;
                float trueV = u * v1 + v * v2 + w * v3;
                float r, g, b;
                tex.Sample(trueU, trueV, r, g, b);
                App::DrawPoint(x, scanlineY, r, g, b);
            }
            // App::DrawLine(curx1, scanlineY, curx2, scanlineY);
            curx1 += invslope1;
            curx2 += invslope2;
        }
    };

    auto FillTop = [](
        float vx1, float vy1, float u1, float v1,
        float vx2, float vy2, float u2, float v2,
        float vx3, float vy3, float u3, float v3,
        SimpleTexture& tex
    ) {
        
        float delta = 1;
        float invslope1 = (vx3 - vx1) / (vy3 - vy1);
        float invslope2 = (vx3 - vx2) / (vy3 - vy2);
        invslope1 *= delta;
        invslope2 *= delta;
        float curx1 = vx3;
        float curx2 = vx3;


        if (invslope1 < invslope2) {
            std::swap(invslope1, invslope2);
        }

        for (float scanlineY = vy3; scanlineY > vy1; scanlineY -= delta)
        {
            for (float x = curx1; x < curx2; x += delta) {

                float det = (vy2 - vy3) * (vx1 - vx3) + (vx3 - vx2) * (vy1 - vy3);
                assert(abs(det) > std::numeric_limits<float>::epsilon(), "not possible");
                float u = ((vy2 - vy3) * (x - vx3) + (vx3 - vx2) * (scanlineY - vy3)) / det;
                float v = ((vy3 - vy1) * (x - vx3) + (vx1 - vx3) * (scanlineY - vy3)) / det;
                float w = 1 - u - v;
                float trueU = u * u1 + v * u2 + w * u3;
                float trueV = u * v1 + v * v2 + w * v3;

                float r, g, b;
                tex.Sample(trueU, trueV, r, g, b);
                App::DrawPoint(x, scanlineY, r, g, b);
            }
            // App::DrawLine(curx1, scanlineY, curx2, scanlineY);
            curx1 -= invslope1;
            curx2 -= invslope2;
        }


    };

    // trivial reject 
    if ((y1 == y2 && y2 == y3) ||  y3 - y1 <= 1.0) {
        return;
    }

    // top triangle exist
    if (y2 == y3) {
        FillBottom(
            x1, y1, u1, v1,
            x2, y2, u2, v2,
            x3, y3, u3, v3,
            tex
        );
    }
    // bottom triangle exist
    else if (y1 == y2) {
        FillTop(
            x1, y1, u1, v1,
            x2, y2, u2, v2,
            x3, y3, u3, v3,
            tex
        );
    }
    else {
        float t = (y2 - y3) / (y1 - y3);
        float x4 = x3 + t * (x1 - x3);
        float y4 = y2;
        float u4 = u3 + t * (u1 - u3);
        float v4 = v3 + t * (v1 - v3);

        FillBottom(
            x1, y1, u1, v1,
            x2, y2, u2, v2,
            x3, y3, u3, v3,
            tex
        );
        FillTop(
            x2, y2, u2, v2,
            x4, y4, u4, v4,
            x3, y3, u3, v3,
            tex
        );
    }
}


void Renderer::DebugDraw(const Triangle& tri)
{
    Vec3 v1 = tri.verts[0].pos;
    Vec3 v2 = tri.verts[1].pos;
    Vec3 v3 = tri.verts[2].pos;

    App::DrawLine(v1.x, v1.y, v2.x, v2.y, 1, 0, 0);
    App::DrawLine(v1.x, v1.y, v3.x, v3.y, 1, 0, 0);    
    App::DrawLine(v2.x, v2.y, v3.x, v3.y, 1, 0, 0);
}

void Renderer::Render()
{
    Camera& cam = GetFirstComponent<Camera>(gCoordinator);
    Vec3 light_direction = Vec3(0, -1.0, 0.0f);
    std::vector<Triangle> TrianglesToRaster;
    //TODO Refactor texture system
    SimpleTexture tex;
   
    // Geometric pipeline
    for (Entity const& e : Visit<Mesh, Transform, SimpleTexture>(gCoordinator)) {

        // std::cout << "Entity ID found: " << e << std::endl;
        Mesh& mesh = gCoordinator.GetComponent<Mesh>(e);
        Transform& t = gCoordinator.GetComponent<Transform>(e);
        tex = gCoordinator.GetComponent<SimpleTexture>(e);

        for (Triangle& tri : mesh.tris) {
            // transform triangle by model matrix
            Triangle translation = tri.transform(t);
            Vec3 normal;

            if (!mesh.hasNormal) {
                Vec3 lineA = translation.verts[1].pos - translation.verts[0].pos;
                Vec3 lineB = translation.verts[2].pos - translation.verts[0].pos;
                normal = lineA.Cross(lineB);
                normal.Normalize();
                translation.verts[0].normal = normal;
                translation.verts[1].normal = normal;
                translation.verts[2].normal = normal;
            }
            else {
                normal = (translation.verts[0].normal + translation.verts[1].normal + translation.verts[2].normal) * (1 / 3);
            }
            // back face culling
            if (normal.Dot(translation.verts[0].pos - cam.pos) < 0)
            {
                // transform triangle into camera space
                Triangle view = translation.transform(cam.world_to_cam);
                // clip against the near plane
                auto clipped = ClipTriangle(Vec3(0, 0, cam.nearplane), Vec3(0, 0, 1), view);
                TrianglesToRaster.insert(TrianglesToRaster.end(), clipped.begin(), clipped.end());
            }
        }
    }


    PainterSort(TrianglesToRaster);

    std::cout << "Triangle To Raster Size " << TrianglesToRaster.size() << std::endl;

    for (Triangle& triangle : TrianglesToRaster) {

        triangle.verts[0].pos = (cam.proj * Vec4(triangle.verts[0].pos)).ToVec3();
        triangle.verts[1].pos = (cam.proj * Vec4(triangle.verts[1].pos)).ToVec3();
        triangle.verts[2].pos = (cam.proj * Vec4(triangle.verts[2].pos)).ToVec3();

        triangle.verts[0].pos.x = (triangle.verts[0].pos.x + 1) * 0.5 * APP_VIRTUAL_WIDTH;
        triangle.verts[0].pos.y = (triangle.verts[0].pos.y + 1) * 0.5 * APP_VIRTUAL_HEIGHT;


        triangle.verts[1].pos.x = (triangle.verts[1].pos.x + 1) * 0.5 * APP_VIRTUAL_WIDTH;
        triangle.verts[1].pos.y = (triangle.verts[1].pos.y + 1) * 0.5 * APP_VIRTUAL_HEIGHT;

        triangle.verts[1].pos.x = (triangle.verts[2].pos.x + 1) * 0.5 * APP_VIRTUAL_WIDTH;
        triangle.verts[1].pos.y = (triangle.verts[2].pos.y + 1) * 0.5 * APP_VIRTUAL_HEIGHT;


        // clip against 4 planes
        std::list<Triangle> clipped;
        clipped.push_back(triangle);
        int clipcount = 1;

        for (int sideID = 0; sideID < 4; sideID++) {
            while (clipcount > 0)
            {
                Triangle test = clipped.front();
                clipped.pop_front();
                clipcount--;
                std::vector<Triangle> boxClipped;

                switch (sideID) 
                {
                    case 0: boxClipped = ClipTriangle(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), test); break;
                    case 1: boxClipped = ClipTriangle(Vec3(0.0f, APP_VIRTUAL_HEIGHT - 1, 0.0f), Vec3(0.0f, -1.0f, 0.0f), test); break;
                    case 2: boxClipped = ClipTriangle(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), test); break;
                    case 3: boxClipped = ClipTriangle(Vec3(APP_VIRTUAL_WIDTH - 1, 0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), test); break;
                }
                clipped.insert(clipped.end(), boxClipped.begin(), boxClipped.end());
            }
            clipcount = clipped.size();
        }

        for (const Triangle& clip : clipped) {
            
            // RenderTriangle(
            //     clip.verts[0].pos.x, clip.verts[0].pos.y, clip.verts[0].tex.x, clip.verts[0].tex.y,
            //     clip.verts[1].pos.x, clip.verts[1].pos.y, clip.verts[1].tex.x, clip.verts[1].tex.y,
            //     clip.verts[2].pos.x, clip.verts[2].pos.y, clip.verts[2].tex.x, clip.verts[2].tex.y,
            //     tex
            // );

            DebugDraw(clip);
        
            //App::DrawTriangle(
            //    clip.verts[0].pos[0].x, clip.verts[0].pos[0].y,
            //    clip.verts[0].pos[1].x, clip.verts[0].pos[1].y,
            //    clip.verts[0].pos[2].x, clip.verts[0].pos[2].y,
            //    1, 1, 1
            //);
        }        
    }
    

}

void Renderer::PainterSort(std::vector<Triangle>& sort)
{
    std::sort(sort.begin(), sort.end(),
    [](Triangle& t1, Triangle& t2) {
        float z1 = (t1.verts[0].pos.z + t1.verts[1].pos.z + t1.verts[2].pos.z) / 3;
        float z2 = (t2.verts[0].pos.z + t2.verts[1].pos.z + t2.verts[2].pos.z) / 3;
        return z1 > z2;
    });
}



