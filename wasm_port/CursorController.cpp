#include "stdafx.h"

#include "CursorController.h"
#include "Camera.h"
#include "ECSManager.h"
#include "Mesh.h"
#include "RigidBody.h"
#include "app.h"

extern ECSManager ECS;

void CursorController::Setup()
{
    m_Cam = ECS.GetResource<Camera>();
    m_Cursor = ECS.GetResource<Cursor>();
}

void CursorController::Update()
{
    float mouseX, mouseY;
    App::GetMousePos(mouseX, mouseY);
    Vec3 planePt = Vec3(0, 0, 0);
    Vec3 planeNormal = Vec3(0, 0, -1);
    Vec3 worldPoint = m_Cam->ScreenSpaceToWorldPoint(mouseX, mouseY, planePt, planeNormal);
    
    m_Cursor->X = worldPoint.X;
    m_Cursor->Y = worldPoint.Y;
    
    m_Cursor->ScreenX = mouseX;
    m_Cursor->ScreenY = mouseY;
    
    m_Cursor->CursorEntity = NULL_ENTITY;
    
    for (auto& id : ECS.Visit<Transform, RigidBody, Mesh>())
    {
        RigidBody& rb = ECS.GetComponent<RigidBody>(id);
        bool mouseCollide = AABBPoint(rb.RigidBodyAABB, worldPoint.X, worldPoint.Y);
        if (mouseCollide)
        {
            m_Cursor->CursorEntity = id;
        }
    }
}
