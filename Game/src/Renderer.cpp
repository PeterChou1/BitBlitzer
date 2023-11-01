#include "stdafx.h"
#include <iostream>
#include <algorithm>
#include "../App/AppSettings.h"
#include "../app/app.h"
#include "Quat.h"
#include "Renderer.h"
#include "Utils.h"



constexpr double ASPECT_RATIO = APP_VIRTUAL_WIDTH / APP_VIRTUAL_HEIGHT;


void RendererCPU::Init()
{
	Mesh cube;
    //cube.tris = {
    //    // SOUTH
    //    Triangle(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 1.0f, 0.0f)),
    //    Triangle(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f)),
    //
    //    // EAST
    //    Triangle(Vec3(1.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f)),
    //    Triangle(Vec3(1.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 0.0f, 1.0f)),
    //
    //    // NORTH
    //    Triangle(Vec3(1.0f, 0.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0f, 1.0f, 1.0f)),
    //    Triangle(Vec3(1.0f, 0.0f, 1.0f), Vec3(0.0f, 1.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f)),
    //
    //    // WEST
    //    Triangle(Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f)),
    //    Triangle(Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f)),
    //
    //    // TOP
    //    Triangle(Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f)),
    //    Triangle(Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 0.0f)),
    //
    //    // BOTTOM
    //    Triangle(Vec3(1.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 0.0f)),
    //    Triangle(Vec3(1.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f)),
    //};
    Utils::LoadFromObjectFile("./Assets/ship.obj", cube);
    meshes.push_back(cube);
    proj.PerspectiveOpenGL(90.0f, ASPECT_RATIO, 0.1, 1000);

}

void RendererCPU::Render()
{

    std::vector<Triangle> TrianglesToRaster;

    for (Mesh& mesh : meshes) {

        for (Triangle& tri : mesh.tris) {
            Quat quatY = Quat(Vec3(0, 1, 0), 1.0f * 3.141f / 180.0f);
            tri.tri[0] = quatY.RotatePoint(tri.tri[0]);
            tri.tri[1] = quatY.RotatePoint(tri.tri[1]);
            tri.tri[2] = quatY.RotatePoint(tri.tri[2]);

            Quat quatX = Quat(Vec3(1, 0, 0), 2.0f * 3.141f / 180.0f);
            tri.tri[0] = quatX.RotatePoint(tri.tri[0]);
            tri.tri[1] = quatX.RotatePoint(tri.tri[1]);
            tri.tri[2] = quatX.RotatePoint(tri.tri[2]);

            Triangle translation = tri;

            translation.tri[0].z += 8.0f;
            translation.tri[1].z += 8.0f;
            translation.tri[2].z += 8.0f;

            Vec3 lineA = translation.tri[1] - translation.tri[0];
            Vec3 lineB = translation.tri[2] - translation.tri[0];
            Vec3 camera = Vec3(0, 0, 0);
            Vec3 normal = lineA.Cross(lineB);
            normal.Normalize();
            

            if (normal.Dot(translation.tri[0] - camera) < 0) 
            {
                TrianglesToRaster.push_back(tri);
            }
        }
    }

    PainterSort(TrianglesToRaster);

    for (Triangle& tri : TrianglesToRaster) {

        Vec3 light_direction = Vec3(0, 0, -1.0f);
        Vec3 lineA = tri.tri[1] - tri.tri[0];
        Vec3 lineB = tri.tri[2] - tri.tri[0];
        Vec3 normal = lineA.Cross(lineB);
        normal.Normalize();
        float dp = normal.Dot(light_direction);

        Triangle translation = tri;

        translation.tri[0].z += 8.0f;
        translation.tri[1].z += 8.0f;
        translation.tri[2].z += 8.0f;

        Triangle projected;
        projected.tri[0] = (proj * Vec4(translation.tri[0])).ToVec3();
        projected.tri[1] = (proj * Vec4(translation.tri[1])).ToVec3();
        projected.tri[2] = (proj * Vec4(translation.tri[2])).ToVec3();

        projected.tri[0].x = (projected.tri[0].x + 1) * 0.5 * APP_VIRTUAL_WIDTH;
        projected.tri[0].y = (projected.tri[0].y + 1) * 0.5 * APP_VIRTUAL_HEIGHT;


        projected.tri[1].x = (projected.tri[1].x + 1) * 0.5 * APP_VIRTUAL_WIDTH;
        projected.tri[1].y = (projected.tri[1].y + 1) * 0.5 * APP_VIRTUAL_HEIGHT;


        projected.tri[2].x = (projected.tri[2].x + 1) * 0.5 * APP_VIRTUAL_WIDTH;
        projected.tri[2].y = (projected.tri[2].y + 1) * 0.5 * APP_VIRTUAL_HEIGHT;


        //std::cout << "original: " << tri.tri[0].toString() << ", " << tri.tri[1].toString() << ", " << tri.tri[2].toString() << "\n";
        //std::cout << "projected: " << projected.tri[0].toString() << ", " << projected.tri[1].toString() << ", " << projected.tri[2].toString() << "\n";

        App::DrawLine(
            projected.tri[0].x, projected.tri[0].y,
            projected.tri[1].x, projected.tri[1].y,
            1, 0, 0
        );

        App::DrawLine(
            projected.tri[0].x, projected.tri[0].y,
            projected.tri[2].x, projected.tri[2].y,
            1, 0, 0
        );


        App::DrawLine(
            projected.tri[1].x, projected.tri[1].y,
            projected.tri[2].x, projected.tri[2].y,
            1, 0, 0
        );

        App::DrawTriangle(
            projected.tri[0].x, projected.tri[0].y,
            projected.tri[1].x, projected.tri[1].y,
            projected.tri[2].x, projected.tri[2].y,
            dp, dp, dp
        );
    }
}

void RendererCPU::PainterSort(std::vector<Triangle>& sort)
{
    std::sort(sort.begin(), sort.end(), 
    [](Triangle& t1, Triangle& t2) {
        float z1 = (t1.tri[0].z + t1.tri[1].z + t1.tri[2].z) / 3;
        float z2 = (t2.tri[0].z + t2.tri[1].z + t2.tri[2].z) / 3;
        return z1 > z2;
    });
}
