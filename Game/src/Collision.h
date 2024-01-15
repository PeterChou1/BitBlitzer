#pragma once
#include "Manifolds.h"
#include "RigidBody.h"


void Circle2Circle(Manifold& m, RigidBody& A, RigidBody& B);

void Polygon2Polygon(Manifold& m, RigidBody& A, RigidBody& B);

void Circle2Polygon(Manifold& m, RigidBody& A, RigidBody& B);

void Polygon2Circle(Manifold& m, RigidBody& A, RigidBody& B);