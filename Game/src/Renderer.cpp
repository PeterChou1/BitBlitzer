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

    int insidePtCount = 0;
    int outsidePtCount = 0;
    
    Vertex* insideVtx[3];
    Vertex* outsideVtx[3];
    std::vector<Triangle> clippedTriangle;

    for (int i = 0; i < 3; i++) {
        float dist = Dist(clip.verts[i].pos, planeNormal, planePoint);
        if (dist >= 0) 
        {
            insideVtx[insidePtCount] = &clip.verts[i];
            insidePtCount++;
        }
        else 
        {
            outsideVtx[outsidePtCount] = &clip.verts[i];
            outsidePtCount++;
        }
    }
    

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

        newTri.verts[0] = *insideVtx[0];
        newTri.verts[1].pos = IntersectPlane(planePoint, planeNormal, newTri.verts[0].pos, (*outsideVtx[0]).pos, t);
        newTri.verts[1].tex = (*insideVtx[0]).tex + ((*outsideVtx[0]).tex - (*insideVtx[0]).tex) * t;
        newTri.verts[1].invW = (*insideVtx[0]).invW + ((*outsideVtx[0]).invW - (*insideVtx[0]).invW) * t;
        newTri.verts[2].pos = IntersectPlane(planePoint, planeNormal, newTri.verts[0].pos, (*outsideVtx[1]).pos, t);
        newTri.verts[2].tex = (*insideVtx[0]).tex + ((*outsideVtx[1]).tex - (*insideVtx[0]).tex) * t;
        newTri.verts[2].invW = (*insideVtx[0]).invW + ((*outsideVtx[1]).invW - (*insideVtx[0]).invW) * t;

        clippedTriangle.emplace_back(newTri);

        return clippedTriangle;
    }

    if (insidePtCount == 2 && outsidePtCount == 1)
    {
        Triangle tri1 = clip, tri2 = clip;
        float t1 = 0, t2 = 0;

        tri1.verts[0] = *insideVtx[0];
        tri1.verts[1] = *insideVtx[1];
        tri1.verts[2].pos = IntersectPlane(planePoint, planeNormal, (*insideVtx[0]).pos, (*outsideVtx[0]).pos, t1);
        tri1.verts[2].tex = (*insideVtx[0]).tex + ((*outsideVtx[0]).tex - (*insideVtx[0]).tex) * t1;
        tri1.verts[2].invW = (*insideVtx[0]).invW + ((*outsideVtx[0]).invW - (*insideVtx[0]).invW) * t1;

        tri2.verts[0] = *insideVtx[1];
        tri2.verts[1] = tri1.verts[2];
        tri2.verts[2].pos = IntersectPlane(planePoint, planeNormal, (*insideVtx[1]).pos, (*outsideVtx[0]).pos, t2);
        tri2.verts[2].tex = (*insideVtx[1]).tex + ((*outsideVtx[0]).tex - (*insideVtx[1]).tex) * t2;
        tri2.verts[2].invW = (*insideVtx[1]).invW + ((*outsideVtx[0]).invW - (*insideVtx[1]).invW) * t2;

        clippedTriangle.emplace_back(tri1);
        clippedTriangle.emplace_back(tri2);
        return clippedTriangle;
    }

    assert(false, "unreachable code");
}

void ComputeBarycentricCoordinates(const Vertex& v1, const Vertex& v2, const Vertex& v3, float x, float y, float& alpha, float& beta, float& gamma) {
    float det = (v2.pos.y - v3.pos.y) * (v1.pos.x - v3.pos.x) + (v3.pos.x - v2.pos.x) * (v1.pos.y - v3.pos.y);
    alpha = ((v2.pos.y - v3.pos.y) * (x - v3.pos.x) + (v3.pos.x - v2.pos.x) * (y - v3.pos.y)) / det;
    beta = ((v3.pos.y - v1.pos.y) * (x - v3.pos.x) + (v1.pos.x - v3.pos.x) * (y - v3.pos.y)) / det;
    gamma = 1.0f - alpha - beta;
}

void FillBottom(Vertex& v1, Vertex& v2, Vertex& v3, SimpleTexture& tex) {

    float delta = 0.33;
    float invslope1 = (v2.pos.x - v1.pos.x) / (v2.pos.y - v1.pos.y);
    float invslope2 = (v3.pos.x - v1.pos.x) / (v3.pos.y - v1.pos.y);
    invslope1 *= delta;
    invslope2 *= delta;


    float curx1 = v1.pos.x;
    float curx2 = v1.pos.x;

    if (invslope1 > invslope2) {
        std::swap(invslope1, invslope2);
    }

    for (float scanlineY = v1.pos.y; scanlineY <= v2.pos.y; scanlineY += delta) {
        for (float x = curx1; x < curx2; x += delta) {
            // ... interpolate u and v (texture coordinates) and draw point
            float alpha, beta, gamma;
            ComputeBarycentricCoordinates(v1, v2, v3, x, scanlineY, alpha, beta, gamma);
            float u = alpha * v1.tex.x + beta * v2.tex.x + gamma * v3.tex.x;
            float v = alpha * v1.tex.y + beta * v2.tex.y + gamma * v3.tex.y;
            float z = alpha * v1.invW + beta * v2.invW + gamma * v3.invW;
            u /= z;
            v /= z;
            float r, g, b;
            tex.Sample(u, v, r, g, b);
            App::DrawPoint(x, scanlineY, r, g, b);
        }
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

void FillTop(Vertex& v1, Vertex& v2, Vertex& v3, SimpleTexture& tex) {

    float delta = 0.33;
    float invslope1 = (v3.pos.x - v1.pos.x) / (v3.pos.y - v1.pos.y);
    float invslope2 = (v3.pos.x - v2.pos.x) / (v3.pos.y - v2.pos.y);
    invslope1 *= delta;
    invslope2 *= delta;

    float curx1 = v3.pos.x;
    float curx2 = v3.pos.x;

    if (invslope1 < invslope2) {
        std::swap(invslope1, invslope2);
    }

    for (float scanlineY = v3.pos.y; scanlineY > v1.pos.y; scanlineY -= delta) {
        for (float x = curx1; x < curx2; x += delta) {
            // ... interpolate u and v (texture coordinates) and draw point
            float alpha, beta, gamma;
            ComputeBarycentricCoordinates(v1, v2, v3, x, scanlineY, alpha, beta, gamma);
            float u = alpha * v1.tex.x + beta * v2.tex.x + gamma * v3.tex.x;
            float v = alpha * v1.tex.y + beta * v2.tex.y + gamma * v3.tex.y;
            float z = alpha * v1.invW + beta * v2.invW + gamma * v3.invW;
            u /= z;
            v /= z;
            float r, g, b;
            tex.Sample(u, v, r, g, b);
            App::DrawPoint(x, scanlineY, r, g, b);
        }
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}




void Renderer::RenderTriangle(Triangle& tri, SimpleTexture& tex) {
    // Sort vertices by y-coordinate
    std::sort(std::begin(tri.verts), std::end(tri.verts),
        [](const Vertex& a, const Vertex& b) {
            return a.pos.y < b.pos.y;
        }
    );

    Vertex& v1 = tri.verts[0];
    Vertex& v2 = tri.verts[1];
    Vertex& v3 = tri.verts[2];

    // trivial reject
    if ((v1.pos.y == v2.pos.y && v2.pos.y == v3.pos.y) || v3.pos.y - v1.pos.y <= 1.0) {
        return;
    }

    if (v2.pos.y == v3.pos.y) {
        FillBottom(v1, v2, v3, tex);
    }
    else if (v1.pos.y == v2.pos.y) {
        FillTop(v1, v2, v3, tex);
    }
    else {
        // Interpolate to find the fourth vertex
        float t = (v2.pos.y - v3.pos.y) / (v1.pos.y - v3.pos.y);
        Vec3 pos4 = Vec3(v3.pos.x + (v1.pos.x - v3.pos.x) * t, v2.pos.y, 0);
        Vec2 tex4 = v3.tex + (v1.tex - v3.tex) * t;
        float w = v3.invW + (v1.invW - v3.invW) * t;
        Vertex v4(pos4, tex4);
        v4.invW = w;

        FillBottom(v1, v2, v3, tex);
        FillTop(v2, v4, v3, tex);
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


    for (Triangle& triangle : TrianglesToRaster) {

        Vec4 v1 = cam.proj * Vec4(triangle.verts[0].pos);
        Vec4 v2 = cam.proj * Vec4(triangle.verts[1].pos);
        Vec4 v3 = cam.proj * Vec4(triangle.verts[2].pos);


        // Perspective division
        triangle.verts[0].pos = v1.ToVec3();
        triangle.verts[1].pos = v2.ToVec3();
        triangle.verts[2].pos = v3.ToVec3();

        triangle.verts[0].tex /= v1.w;
        triangle.verts[1].tex /= v2.w;
        triangle.verts[2].tex /= v3.w;

        triangle.verts[0].invW = 1.0f / v1.w;
        triangle.verts[1].invW = 1.0f / v2.w;
        triangle.verts[2].invW = 1.0f / v3.w;

        triangle.verts[0].pos.x = (triangle.verts[0].pos.x + 1) * 0.5 * APP_VIRTUAL_WIDTH;
        triangle.verts[0].pos.y = (triangle.verts[0].pos.y + 1) * 0.5 * APP_VIRTUAL_HEIGHT;


        triangle.verts[1].pos.x = (triangle.verts[1].pos.x + 1) * 0.5 * APP_VIRTUAL_WIDTH;
        triangle.verts[1].pos.y = (triangle.verts[1].pos.y + 1) * 0.5 * APP_VIRTUAL_HEIGHT;

        triangle.verts[2].pos.x = (triangle.verts[2].pos.x + 1) * 0.5 * APP_VIRTUAL_WIDTH;
        triangle.verts[2].pos.y = (triangle.verts[2].pos.y + 1) * 0.5 * APP_VIRTUAL_HEIGHT;


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

        for (Triangle& clip : clipped) {
            
            RenderTriangle(clip, tex);

            //DebugDraw(clip);
        
            //App::DrawTriangle(
            //    clip.verts[0].pos.x, clip.verts[0].pos.y,
            //    clip.verts[1].pos.x, clip.verts[1].pos.y,
            //    clip.verts[2].pos.x, clip.verts[2].pos.y,
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



