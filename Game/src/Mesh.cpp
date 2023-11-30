#include "stdafx.h"
#include "Mesh.h"

const Triangle Triangle::transform(const Transform& rhs)
{
	Triangle tri = *this;
	tri.tri[0] *= rhs;
	tri.tri[1] *= rhs;
	tri.tri[2] *= rhs;
	return tri;
}