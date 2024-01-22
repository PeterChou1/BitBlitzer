#include "stdafx.h"
#include <cmath>
#include "ExtendedGraphics.h"
#include "../App/app.h"

void DrawThickLine(float sx, float sy, float ex, float ey, float thickness, float r, float g, float b)
{
    // Calculate the perpendicular angle to the line
    float angle = atan2(ey - sy, ex - sx) + 3.141f / 2;

    // Calculate offsets for the thickness
    for (float i = -thickness / 2; i <= thickness / 2; i += 0.5f) {
        float offsetX = i * cos(angle);
        float offsetY = i * sin(angle);

        // Draw lines next to each other to simulate thickness
        App::DrawLine(sx + offsetX, sy + offsetY, ex + offsetX, ey + offsetY, r, g, b);
    }
}

void DrawArrow(float sx, float sy, float ex, float ey, float thickness, float r, float g, float b)
{
    // Draw the main line of the arrow
    DrawThickLine(sx, sy, ex, ey, thickness, r, g, b);

    // Parameters for the arrowhead
    const float arrowHeadLength = 20.0f; // Length of the arrow head lines
    const float arrowHeadAngle = 45.0f;  // Angle of the arrow head lines in degrees

    // Calculate the angle of the main line
    float angle = atan2(ey - sy, ex - sx);

    // Calculate the two end points of the arrowhead lines
    float arrowHeadX1 = ex - arrowHeadLength * cos(angle - 3.141f * arrowHeadAngle / 180.0f);
    float arrowHeadY1 = ey - arrowHeadLength * sin(angle - 3.141f * arrowHeadAngle / 180.0f);

    float arrowHeadX2 = ex - arrowHeadLength * cos(angle + 3.141f * arrowHeadAngle / 180.0f);
    float arrowHeadY2 = ey - arrowHeadLength * sin(angle + 3.141f * arrowHeadAngle / 180.0f);

    // Draw the two lines of the arrowhead
    DrawThickLine(ex, ey, arrowHeadX1, arrowHeadY1, thickness, r, g, b);
    DrawThickLine(ex, ey, arrowHeadX2, arrowHeadY2, thickness, r, g, b);
}
