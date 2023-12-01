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



void Renderer::Setup()
{
}

void Renderer::Render()
{

    std::set<Entity> cams = Visit<Camera>(gCoordinator);
    assert(cams.size() == 1, "no camera registered");
    Entity const& e = *cams.begin();
    Camera& cam = gCoordinator.GetComponent<Camera>(e);


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

            if (normal.Dot(translation.tri[0] - cam.pos) < 0)
            {
                TrianglesToRaster.push_back(translation);
            }
        }
    }

    PainterSort(TrianglesToRaster);

    for (Triangle& tri : TrianglesToRaster) {

        Vec3 light_direction = Vec3(0, 0, -1.0f);

        float dp = tri.normal.Dot(light_direction);

        Triangle translation = tri;

        Triangle view;
        Triangle projected;

        view = translation.transform(cam.world_to_cam);

        projected.tri[0] = (cam.proj * Vec4(view.tri[0])).ToVec3();
        projected.tri[1] = (cam.proj * Vec4(view.tri[1])).ToVec3();
        projected.tri[2] = (cam.proj * Vec4(view.tri[2])).ToVec3();

        projected.tri[0].x = (projected.tri[0].x + 1) * 0.5 * APP_VIRTUAL_WIDTH;
        projected.tri[0].y = (projected.tri[0].y + 1) * 0.5 * APP_VIRTUAL_HEIGHT;


        projected.tri[1].x = (projected.tri[1].x + 1) * 0.5 * APP_VIRTUAL_WIDTH;
        projected.tri[1].y = (projected.tri[1].y + 1) * 0.5 * APP_VIRTUAL_HEIGHT;

        projected.tri[2].x = (projected.tri[2].x + 1) * 0.5 * APP_VIRTUAL_WIDTH;
        projected.tri[2].y = (projected.tri[2].y + 1) * 0.5 * APP_VIRTUAL_HEIGHT;


        // App::DrawLine(
        //     projected.tri[0].x, projected.tri[0].y,
        //     projected.tri[1].x, projected.tri[1].y,
        //     1, 0, 0
        // );
        //     
        // App::DrawLine(
        //     projected.tri[0].x, projected.tri[0].y,
        //     projected.tri[2].x, projected.tri[2].y,
        //     1, 0, 0
        // );
        //     
        //     
        // App::DrawLine(
        //     projected.tri[1].x, projected.tri[1].y,
        //     projected.tri[2].x, projected.tri[2].y,
        //     1, 0, 0
        // );

        App::DrawTriangle(
            projected.tri[0].x, projected.tri[0].y,
            projected.tri[1].x, projected.tri[1].y,
            projected.tri[2].x, projected.tri[2].y,
            dp, dp, dp
        );
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



