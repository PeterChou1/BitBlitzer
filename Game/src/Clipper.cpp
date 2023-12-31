#include "stdafx.h"
#include <cassert>
#include "Clipper.h"

float Dot(uint8_t planeId, const Vec4& v)
{
    switch (planeId) {
    case LEFT_PLANE:
        return v.x + v.w;        /* v * (1 0 0 1) left */
    case RIGHT_PLANE:
        return -v.x + v.w;       /* v * (-1 0 0 1) right */
    case DOWN_PLANE:
        return v.y + v.w;        /* v * (0 1 0 1) down*/
    case UP_PLANE:
        return -v.y + v.w;       /* v * (0 -1 0 1) up*/
    case FAR_PLANE:
        return v.z + v.w;        /* v * (0 0 1 1) far */
    case NEAR_PLANE:
        return -v.z + v.w;       /* v * (0 0 -1 1) near */
    default:
        assert(false && "unreachable code");
    }
}

float PointToPlane(uint8_t planeId, const Vec4& a, const Vec4& b)
{
    float alpha = Dot(planeId, a);
    float beta = Dot(planeId, b);
    return alpha / (alpha - beta);
}


void Lerp(const Point& a, const Point& b, float alpha, Point& out)
{
    float a1 = 1.0f - alpha;
    out.position = a.position * a1 + b.position * alpha;
    out.weights = a.weights * a1 + b.weights * alpha;
}


uint8_t OutCode(const Vec4& v)
{

    uint8_t outcode = 0;

    if (Dot(LEFT_PLANE, v) < 0) outcode |= LEFT_PLANE;
    if (Dot(RIGHT_PLANE, v) < 0) outcode |= RIGHT_PLANE;
    if (Dot(DOWN_PLANE, v) < 0) outcode |= DOWN_PLANE;
    if (Dot(UP_PLANE, v) < 0) outcode |= UP_PLANE;
    if (Dot(FAR_PLANE, v) < 0) outcode |= FAR_PLANE;
    if (Dot(NEAR_PLANE, v) < 0) outcode |= NEAR_PLANE;

    return outcode;
}

__forceinline std::vector<Point>
ClipPlane(uint32_t planeid, std::vector<Point>& points)
{

    std::vector<Point> outPoints;

    for (int i = 0, j = 1; i < points.size(); i++, j++)
    {
        if (i == points.size() - 1) j = 0;

        const auto& Apoint = points[i];
        const auto& Bpoint = points[j];


        float t = PointToPlane(planeid, Apoint.position, Bpoint.position);
        Point newpoint{};
        Lerp(points[i], points[j], t, newpoint);

        // is inside

        const auto isInsideA = Dot(planeid, Apoint.position) > 0;
        const auto isInsideB = Dot(planeid, Bpoint.position) > 0;


        if (isInsideA) {
            if (isInsideB) {
                outPoints.push_back(Bpoint);
            }
            else {
                outPoints.push_back(newpoint);
            }
        }
        else if (isInsideB) {
            outPoints.push_back(newpoint);
            outPoints.push_back(Bpoint);
        }
    }

    return outPoints;
}


std::vector<Triangle> Clip(Triangle& clip)
{

    uint8_t clipcode1 = OutCode(clip.verts[0].proj);
    uint8_t clipcode2 = OutCode(clip.verts[1].proj);
    uint8_t clipcode3 = OutCode(clip.verts[2].proj);

    std::vector<Triangle> clipped;


    if (!(clipcode1 | clipcode2 | clipcode3)) {
        // trivial accept
        clip.PerspectiveDivision();
        clipped.push_back(clip);
    }
    else if (!(clipcode1 & clipcode2 & clipcode3)) {

        Vertex v0 = clip.verts[0];
        Vertex v1 = clip.verts[1];
        Vertex v2 = clip.verts[2];


        std::vector<Point> points = {
            Point(clip.verts[0].proj, {1, 0, 0}),
            Point(clip.verts[1].proj, {0, 1, 0}),
            Point(clip.verts[2].proj, {0, 0, 1})
        };
        uint32_t mask = clipcode1 | clipcode2 | clipcode3;
        if (mask & LEFT_PLANE) {
            points = ClipPlane(LEFT_PLANE, points);
        }

        if (mask & RIGHT_PLANE) {
            points = ClipPlane(RIGHT_PLANE, points);
        }

        if (mask & DOWN_PLANE) {
            points = ClipPlane(DOWN_PLANE, points);
        }

        if (mask & UP_PLANE) {
            points = ClipPlane(UP_PLANE, points);
        }

        if (mask & NEAR_PLANE) {
            points = ClipPlane(NEAR_PLANE, points);
        }

        if (mask & FAR_PLANE) {
            points = ClipPlane(FAR_PLANE, points);
        }

        // triangulate the points
        for (int j = 2; j < points.size(); j++)
        {
            Point& p1 = points[0];
            Point& p2 = points[j - 1];
            Point& p3 = points[j];

            Vertex n1 = v0 * p1.weights.x + v1 * p1.weights.y + v2 * p1.weights.z;
            n1.proj = p1.position;
            Vertex n2 = v0 * p2.weights.x + v1 * p2.weights.y + v2 * p2.weights.z;
            n2.proj = p2.position;
            Vertex n3 = v0 * p3.weights.x + v1 * p3.weights.y + v2 * p3.weights.z;
            n3.proj = p3.position;

            Triangle newtri = Triangle(n1, n2, n3);
            newtri.PerspectiveDivision();
            clipped.push_back(newtri);
        }
    }
    // o/w trivial reject
    return clipped;
}


