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
    // All of this might seem confusing but
    // all this function is doing is determining which corners
    // of the tile to trivially reject and accept a triangle
    // in a tile
    // more on this here: https://www.cs.cmu.edu/afs/cs/academic/class/15869-f11/www/readings/abrash09_lrbrast.pdf
    BinIndex = index;
    BinID = id;

    B0 = static_cast<int>(verts[1].Projection.Y - verts[0].Projection.Y);
    C0 = static_cast<int>(verts[1].Projection.X - verts[0].Projection.X);
    B1 = static_cast<int>(verts[2].Projection.Y - verts[1].Projection.Y);
    C1 = static_cast<int>(verts[2].Projection.X - verts[1].Projection.X);
    B2 = static_cast<int>(verts[0].Projection.Y - verts[2].Projection.Y);
    C2 = static_cast<int>(verts[0].Projection.X - verts[2].Projection.X);

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

    // triangle has no face early reject
    if (det == 0)
    {
        return false;
    }

    invDet = 1.0f / static_cast<float>(det);

    Vec2 edgeNormal1 = Vec2(static_cast<float>(-B0), static_cast<float>(C0));
    Vec2 edgeNormal2 = Vec2(static_cast<float>(-B1), static_cast<float>(C1));
    Vec2 edgeNoraml3 = Vec2(static_cast<float>(-B2), static_cast<float>(C2));

    // set up bounding box
    maxX = static_cast<int>(std::max<float>(std::max<float>(verts[0].Projection.X, verts[1].Projection.X), verts[2].Projection.X));
    maxY = static_cast<int>(std::max<float>(std::max<float>(verts[0].Projection.Y, verts[1].Projection.Y), verts[2].Projection.Y));
    minX = static_cast<int>(std::min<float>(std::min<float>(verts[0].Projection.X, verts[1].Projection.X), verts[2].Projection.X));
    minY = static_cast<int>(std::min<float>(std::min<float>(verts[0].Projection.Y, verts[1].Projection.Y), verts[2].Projection.Y));


    if (edgeNormal1.X > 0)
    {
        if (edgeNormal1.Y > 0)
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
        if (edgeNormal1.Y > 0)
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

    if (edgeNormal2.X > 0)
    {
        if (edgeNormal2.Y > 0)
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
        if (edgeNormal2.Y > 0)
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

    if (edgeNoraml3.X > 0)
    {
        if (edgeNoraml3.Y > 0)
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
        if (edgeNoraml3.Y > 0)
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
