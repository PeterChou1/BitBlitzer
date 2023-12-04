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


    float d0 = Dist(clip.tri[0], planeNormal, planePoint);
    float d1 = Dist(clip.tri[1], planeNormal, planePoint);
    float d2 = Dist(clip.tri[2], planeNormal, planePoint);

    
    if (d0 >= 0) 
    { 
        insidePts[insidePtCount] = &clip.tri[0];
        insideTex[insidePtCount]  = &clip.tex[0];
        insidePtCount++;
    }
    else 
    { 
        outsidePts[outsidePtCount] = &clip.tri[0];
        outsideTex[outsidePtCount] = &clip.tex[0];
        outsidePtCount++;
    };

    if (d1 >= 0) 
    { 
        insidePts[insidePtCount] = &clip.tri[1];
        insideTex[insidePtCount] = &clip.tex[1];
        insidePtCount++;
    }
    else 
    { 
        outsidePts[outsidePtCount] = &clip.tri[1]; 
        outsideTex[outsidePtCount] = &clip.tex[1];
        outsidePtCount++;
    };

    if (d2 >= 0) 
    { 
        insidePts[insidePtCount] = &clip.tri[2]; 
        insideTex[insidePtCount] = &clip.tex[2];
        insidePtCount++;
    }
    else 
    { 
        outsidePts[outsidePtCount] = &clip.tri[2];
        outsideTex[outsidePtCount] = &clip.tex[2];
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
        newTri.tri[0] = *insidePts[0];
        newTri.tex[0] = *insideTex[0];

        newTri.tri[1] = IntersectPlane(planePoint, planeNormal, newTri.tri[0], *outsidePts[0], t);
        newTri.tex[1] = *insideTex[0] + (*outsideTex[0] - *insideTex[0]) * t;

        newTri.tri[2] = IntersectPlane(planePoint, planeNormal, newTri.tri[0], *outsidePts[1], t);
        newTri.tex[2] = *insideTex[0] + (*outsideTex[1] - *insideTex[0]) * t;

        clippedTriangle.emplace_back(newTri);

        return clippedTriangle;
    }

    if (insidePtCount == 2 && outsidePtCount == 1) 
    {
        Triangle tri1 = clip, tri2 = clip;
        float t = 0;
        tri1.tri[0] = *insidePts[0];
        tri1.tex[0] = *insideTex[0];

        tri1.tri[1] = *insidePts[1];
        tri1.tex[1] = *insideTex[1];


        tri1.tri[2] = IntersectPlane(planePoint, planeNormal, *insidePts[0], *outsidePts[0], t);
        tri1.tex[2] = *insideTex[0] + (*outsideTex[0] - *insideTex[0]) * t;


        tri2.tri[0] = *insidePts[1];
        tri2.tex[0] = *insideTex[1];

        tri2.tri[1] = tri1.tri[2];
        tri2.tex[1] = tri1.tex[2];

        tri2.tri[2] = IntersectPlane(planePoint, planeNormal, *insidePts[1], *outsidePts[0], t);
        tri1.tex[2] = *insideTex[1] + (*outsideTex[0] - *insideTex[1]) * t;

        clippedTriangle.emplace_back(tri1);
        clippedTriangle.emplace_back(tri2);
        return clippedTriangle;
    }

    assert(false, "unreachable code");
}




void Renderer::RenderTriangle(
    float x1, float y1, float u1, float v1, 
    float x2, float y2, float u2, float v2, 
    float x3, float y3, float u3, float v3
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
        float vx3, float vy3, float u3, float v3
    ) {

        float delta = 1;
        float invslope1 = (vx2 - vx1) / (vy2 - vy1);
        float invslope2 = (vx3 - vx1) / (vy3 - vy1);
        float uvslope1_u = (u2 - u1) / (vy2 - vy1);
        float uvslope1_v = (v2 - v1) / (vy2 - vy1);
        float uvslope2_u = (u3 - u1) / (vy3 - vy1);
        float uvslope2_v = (v3 - v1) / (vy3 - vy1);

        invslope1 *= delta;
        invslope2 *= delta;

        float curx1 = vx1;
        float curx2 = vx1;
        float curu1 = u1, curv1 = v1;
        float curu2 = u1, curv2 = v1;

        if (invslope1 > invslope2) {
            std::swap(invslope1, invslope2);
        }


        for (float scanlineY = vy1; scanlineY <= vy2; scanlineY += delta)
        {
            for (float x = curx1; x <= curx2; x += delta) {
                App::DrawPoint(x, scanlineY);
            }
            // App::DrawLine(curx1, scanlineY, curx2, scanlineY);
            curx1 += invslope1;
            curx2 += invslope2;
            curu1 += uvslope1_u;
            curv1 += uvslope1_v;
            curu2 += uvslope2_u;
            curv2 += uvslope2_v;
        }
    };

    auto FillTop = [](
        float vx1, float vy1, float u1, float v1,
        float vx2, float vy2, float u2, float v2,
        float vx3, float vy3, float u3, float v3
    ) {

        float delta = 1;
        float invslope1 = (vx3 - vx1) / (vy3 - vy1);
        float invslope2 = (vx3 - vx2) / (vy3 - vy2);
        float uvslope1 = (u3 - u1) / (v3 - v1);
        float uvslope2 = (u3 - u2) / (v3 - v2);

        invslope1 *= delta;
        invslope2 *= delta;

        float curx1 = vx3;
        float curx2 = vx3;

        if (invslope1 < invslope2) {
            std::swap(invslope1, invslope2);
        }

        for (float scanlineY = vy3; scanlineY > vy1; scanlineY -= delta)
        {

            for (float x = curx1; x <= curx2; x += delta) {
                App::DrawPoint(x, scanlineY);
            }
            // App::DrawLine(curx1, scanlineY, curx2, scanlineY);
            curx1 -= invslope1;
            curx2 -= invslope2;
        }
    };

    // trivial reject 
    if ((y1 == y2 && y2 == y3) ||  y3 - y1 <= 1) {
        return;
    }

    // top triangle exist
    if (y2 == y3) {
        FillBottom(
            x1, y1, u1, v1,
            x2, y2, u2, v2,
            x3, y3, u3, v3
        );
    }
    // bottom triangle exist
    else if (y1 == y2) {
        FillTop(
            x1, y1, u1, v1,
            x2, y2, u2, v2,
            x3, y3, u3, v3
        );
    }
    else {
        float x4 = x3 + (((y2 - y3) / (y1 - y3)) * (x1 - x3));
        float y4 = y2;
        float u4 = 0.0;
        float v4 = v2;

        FillBottom(
            x1, y1, u1, v1,
            x2, y2, u2, v2,
            x3, y3, u3, v3
        );
        FillTop(
            x2, y2, u2, v2,
            x4, y4, u4, v4,
            x3, y3, u3, v3
        );
    }
}


void Renderer::DebugDraw(const Triangle& tri)
{
    App::DrawLine(
        tri.tri[0].x, tri.tri[0].y,
        tri.tri[1].x, tri.tri[1].y,
        1, 0, 0
    );
        
    App::DrawLine(
        tri.tri[0].x, tri.tri[0].y,
        tri.tri[2].x, tri.tri[2].y,
        1, 0, 0
    );
        
        
    App::DrawLine(
        tri.tri[1].x, tri.tri[1].y,
        tri.tri[2].x, tri.tri[2].y,
        1, 0, 0
    );
}

void Renderer::Render()
{

    std::set<Entity> cams = Visit<Camera>(gCoordinator);
    assert(cams.size() == 1, "no camera registered");
    Entity const& e = *cams.begin();
    Camera& cam = gCoordinator.GetComponent<Camera>(e);
    Vec3 light_direction = Vec3(0, -1.0, 0.0f);
    std::vector<Triangle> TrianglesToRaster;
   
    // Geometric pipeline
    for (Entity const& e : Visit<Mesh, Transform>(gCoordinator)) {

        // std::cout << "Entity ID found: " << e << std::endl;
        Mesh& mesh = gCoordinator.GetComponent<Mesh>(e);
        Transform& t = gCoordinator.GetComponent<Transform>(e);

        for (Triangle& tri : mesh.tris) {
            Triangle translation = tri.transform(t);

            Vec3 lineA = translation.tri[1] - translation.tri[0];
            Vec3 lineB = translation.tri[2] - translation.tri[0];
            Vec3 normal = lineA.Cross(lineB);
            normal.Normalize();
            translation.normal = normal;

            float dp = translation.normal.Dot(light_direction);
            translation.r = dp;
            translation.g = dp;
            translation.b = dp;
            
            if (normal.Dot(translation.tri[0] - cam.pos) < 0)
            {
                Triangle view = translation.transform(cam.world_to_cam);
                // clip against the near plane
                auto clipped = ClipTriangle(Vec3(0, 0, cam.nearplane), Vec3(0, 0, 1), view);
                TrianglesToRaster.insert(TrianglesToRaster.end(), clipped.begin(), clipped.end());
            }
        }
    }
    PainterSort(TrianglesToRaster);


    for (Triangle& triangle : TrianglesToRaster) {

        triangle.tri[0] = (cam.proj * Vec4(triangle.tri[0])).ToVec3();
        triangle.tri[1] = (cam.proj * Vec4(triangle.tri[1])).ToVec3();
        triangle.tri[2] = (cam.proj * Vec4(triangle.tri[2])).ToVec3();

        triangle.tri[0].x = (triangle.tri[0].x + 1) * 0.5 * APP_VIRTUAL_WIDTH;
        triangle.tri[0].y = (triangle.tri[0].y + 1) * 0.5 * APP_VIRTUAL_HEIGHT;


        triangle.tri[1].x = (triangle.tri[1].x + 1) * 0.5 * APP_VIRTUAL_WIDTH;
        triangle.tri[1].y = (triangle.tri[1].y + 1) * 0.5 * APP_VIRTUAL_HEIGHT;

        triangle.tri[2].x = (triangle.tri[2].x + 1) * 0.5 * APP_VIRTUAL_WIDTH;
        triangle.tri[2].y = (triangle.tri[2].y + 1) * 0.5 * APP_VIRTUAL_HEIGHT;


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
            
            RenderTriangle(
                clip.tri[0].x, clip.tri[0].y, clip.tex[0].x, clip.tex[0].y,
                clip.tri[1].x, clip.tri[1].y, clip.tex[1].x, clip.tex[1].y,
                clip.tri[2].x, clip.tri[2].y, clip.tex[2].x, clip.tex[2].y
            );

            DebugDraw(clip);
        
            //App::DrawTriangle(
            //    clip.tri[0].x, clip.tri[0].y,
            //    clip.tri[1].x, clip.tri[1].y,
            //    clip.tri[2].x, clip.tri[2].y,
            //    1, 1, 1
            //);
        }        
    }
    

}

void Renderer::PainterSort(std::vector<Triangle>& sort)
{
    std::sort(sort.begin(), sort.end(),
    [](Triangle& t1, Triangle& t2) {
        float z1 = (t1.tri[0].z + t1.tri[1].z + t1.tri[2].z) / 3;
        float z2 = (t2.tri[0].z + t2.tri[1].z + t2.tri[2].z) / 3;
        return z1 > z2;
    });
}



