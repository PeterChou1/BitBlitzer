#include "stdafx.h"
#include "Mesh.h"

Triangle Triangle::transform(const Transform& rhs)
{
	Triangle tri = *this;
	tri.verts[0].pos *= rhs;
	tri.verts[1].pos *= rhs;
	tri.verts[2].pos *= rhs;
	tri.verts[0].normal = rhs.rotation.RotatePoint(tri.verts[0].normal);
	tri.verts[1].normal = rhs.rotation.RotatePoint(tri.verts[1].normal);
	tri.verts[2].normal = rhs.rotation.RotatePoint(tri.verts[2].normal);
	return tri;
}