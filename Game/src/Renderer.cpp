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

    Vec3* insidePts[3]; int insidePtCount = 0;
    Vec3* outsidePts[3]; int outsidePtCount = 0;
    std::vector<Triangle> clippedTriangle;


    float d0 = Dist(clip.tri[0], planeNormal, planePoint);
    float d1 = Dist(clip.tri[1], planeNormal, planePoint);
    float d2 = Dist(clip.tri[2], planeNormal, planePoint);

    
    if (d0 >= 0) { insidePts[insidePtCount++] = &clip.tri[0]; }
    else { outsidePts[outsidePtCount++] = &clip.tri[0]; };

    if (d1 >= 0) { insidePts[insidePtCount++] = &clip.tri[1]; }
    else { outsidePts[outsidePtCount++] = &clip.tri[1]; };

    if (d2 >= 0) { insidePts[insidePtCount++] = &clip.tri[2]; }
    else { outsidePts[outsidePtCount++] = &clip.tri[2]; };


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
        Triangle newTri;
        newTri.tri[0] = *insidePts[0];
        newTri.tri[1] = IntersectPlane(planePoint, planeNormal, newTri.tri[0], *outsidePts[0]);
        newTri.tri[2] = IntersectPlane(planePoint, planeNormal, newTri.tri[0], *outsidePts[1]);
        newTri.r = clip.r;
        newTri.g = clip.g;
        newTri.b = clip.b;
        // newTri.r = 1;
        // newTri.g = 0;
        // newTri.b = 0;
        clippedTriangle.emplace_back(newTri);

        return clippedTriangle;
    }

    if (insidePtCount == 2 && outsidePtCount == 1) 
    {
        Triangle tri1, tri2;
        tri1.tri[0] = *insidePts[0];
        tri1.tri[1] = *insidePts[1];
        tri1.tri[2] = IntersectPlane(planePoint, planeNormal, *insidePts[0], *outsidePts[0]);
        tri2.tri[0] = *insidePts[1];
        tri2.tri[1] = tri1.tri[2];
        tri2.tri[2] = IntersectPlane(planePoint, planeNormal, *insidePts[1], *outsidePts[0]);
        tri1.r = clip.r;
        tri1.g = clip.g;
        tri1.b = clip.b;
        tri2.r = clip.r;
        tri2.g = clip.g;
        tri2.b = clip.b;
        //tri1.r = 0;
        //tri1.g = 1;
        //tri1.b = 0;        
        //tri2.r = 0;
        //tri2.b = 0;
        //tri2.g = 1;

        clippedTriangle.emplace_back(tri1);
        clippedTriangle.emplace_back(tri2);
        return clippedTriangle;
    }

    assert(false, "unreachable code");
}

std::vector<Triangle> Renderer::Clip(std::vector<Triangle>& mesh)
{
    return mesh;
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
            // DebugDraw(clip);
        
            App::DrawTriangle(
                clip.tri[0].x, clip.tri[0].y,
                clip.tri[1].x, clip.tri[1].y,
                clip.tri[2].x, clip.tri[2].y,
                clip.r, clip.g, clip.b
            );
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



