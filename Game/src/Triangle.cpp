#include "stdafx.h"
#include "Triangle.h"

void Triangle::PerspectiveDivision()
{
    verts[0].PerspectiveDivision();
    verts[1].PerspectiveDivision();
    verts[2].PerspectiveDivision();
}

bool Triangle::Setup(int id, int index)
{
    BinIndex = index, BinID = id;

    B0 = static_cast<int>(verts[1].proj.y - verts[0].proj.y);
    C0 = static_cast<int>(verts[1].proj.x - verts[0].proj.x);
    B1 = static_cast<int>(verts[2].proj.y - verts[1].proj.y);
    C1 = static_cast<int>(verts[2].proj.x - verts[1].proj.x);
    B2 = static_cast<int>(verts[0].proj.y - verts[2].proj.y);
    C2 = static_cast<int>(verts[0].proj.x - verts[2].proj.x);

    const int det = C2 * -B1 + C1 * B2;

    if (det < 0)
    {
        B0 *= -1;
        C0 *= -1;
        B1 *= -1;
        C1 *= -1;
        B2 *= -1;
        C2 *= -1;
    }

    if (det == 0)
    {
        return false;
    }

    invDet = 1.0f / static_cast<float>(det);

    Vec2 edgeNormal1 = Vec2(-B0, C0);
    Vec2 edgeNormal2 = Vec2(-B1, C1);
    Vec2 edgeNoraml3 = Vec2(-B2, C2);

    // set up bounding box
    maxX = static_cast<int>(std::max<float>(std::max<float>(verts[0].proj.x, verts[1].proj.x), verts[2].proj.x));
    maxY = static_cast<int>(std::max<float>(std::max<float>(verts[0].proj.y, verts[1].proj.y), verts[2].proj.y));
    minX = static_cast<int>(std::min<float>(std::min<float>(verts[0].proj.x, verts[1].proj.x), verts[2].proj.x));
    minY = static_cast<int>(std::min<float>(std::min<float>(verts[0].proj.y, verts[1].proj.y), verts[2].proj.y));


    if (edgeNormal1.x > 0)
    {
        if (edgeNormal1.y > 0)
        {
            rejectIndex0 = 3;
            acceptIndex0 = 0;
        }
        else
        {
            rejectIndex0 = 1;
            acceptIndex0 = 2;
        }
    }
    else
    {
        if (edgeNormal1.y > 0)
        {
            rejectIndex0 = 2;
            acceptIndex0 = 1;
        }
        else
        {
            rejectIndex0 = 0;
            acceptIndex0 = 3;
        }
    }

    if (edgeNormal2.x > 0)
    {
        if (edgeNormal2.y > 0)
        {
            rejectIndex1 = 3;
            acceptIndex1 = 0;
        }
        else
        {
            rejectIndex1 = 1;
            acceptIndex1 = 2;
        }
    }
    else
    {
        if (edgeNormal2.y > 0)
        {
            rejectIndex1 = 2;
            acceptIndex1 = 1;
        }
        else
        {
            rejectIndex1 = 0;
            acceptIndex1 = 3;
        }
    }

    if (edgeNoraml3.x > 0)
    {
        if (edgeNoraml3.y > 0)
        {
            rejectIndex2 = 3;
            acceptIndex2 = 0;
        }
        else
        {
            rejectIndex2 = 1;
            acceptIndex2 = 2;
        }
    }
    else
    {
        if (edgeNoraml3.y > 0)
        {
            rejectIndex2 = 2;
            acceptIndex2 = 1;
        }
        else
        {
            rejectIndex2 = 0;
            acceptIndex2 = 3;
        }
    }

    return det > 0;
}


bool Triangle::CheckInTriangle(Vec2& point) const
{
    float e0 = EdgeFunc0(point);
    float e1 = EdgeFunc1(point);
    float e2 = EdgeFunc2(point);
    return e0 < 0 && e1 < 0 && e2 < 0;
}