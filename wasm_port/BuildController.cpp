#include "stdafx.h"
#include "BuildController.h"

#include "ECSManager.h"
#include "Mesh.h"
#include "RigidBody.h"
#include "Shader.h"
#include "app.h"

extern ECSManager ECS;

void BuildController::Setup()
{
    m_Cursor = ECS.GetResource<Cursor>();
    m_GameState = ECS.GetResource<GameState>();
    m_Selected = NULL_ENTITY;
}

void BuildController::Update(float deltaTime)
{
    m_Accumulate += deltaTime / 1000.0f;
    m_RotateAccumulate += deltaTime / 1000.0f;

    if (App::IsKeyPressed(' ') && m_Selected != NULL_ENTITY)
    {
        ECS.DestroyEntity(m_Selected);
        m_Selected = NULL_ENTITY;
    }
    
    if (m_GameState->State == BuildMode)
    {
        HandleBuild();
        HandleSelect();
    }
}

void BuildController::Render()
{
    if (m_GameState->State == BuildMode)
    {
        if (m_Selected != NULL_ENTITY)
        {
            Mesh& mesh = ECS.GetComponent<Mesh>(m_Selected);
    
            if (mesh.MeshType == SpotFront)
            {
                App::Print(
                    m_Cursor->ScreenX, m_Cursor->ScreenY,
                    "Press E to Let Go"
                );
            }
            else
            {
                App::Print(
                    m_Cursor->ScreenX, m_Cursor->ScreenY,
                    "Press R to Rotate | Press E to Let Go"
                );
            }
        }
        else if (m_Cursor->CursorEntity != NULL_ENTITY)
        {
            App::Print(
                m_Cursor->ScreenX, m_Cursor->ScreenY,
                "Press E to Grab"
            );
        }
    }
}

void BuildController::HandleBuild()
{

    if (m_Accumulate > 0.1f)
    {
        m_Accumulate = 0.0f;
    
        if (App::IsKeyPressed('E') && m_Cursor->CursorEntity != NULL_ENTITY)
        {
            if (m_Selected != NULL_ENTITY)
            {
                RigidBody& rb = ECS.GetComponent<RigidBody>(m_Selected);
                Shader& shader = ECS.GetComponent<Shader>(m_Selected);
                if (rb.IsIntersecting)
                    App::PlaySound("Assets/Sounds/wrongSound.wav");
                else
                {
                    shader.ShaderId = DefaultShader;
                    rb.Collidable = true;
                    rb.Velocity = 0.0f;
                    rb.AngularVelocity = 0.0f;
                    m_Selected = NULL_ENTITY;
                }
            }
            else
            {
                m_Selected = m_Cursor->CursorEntity;
            }
        }
    
        int playerMoney = m_GameState->Turn == Player1Turn ? m_GameState->Player1Money : m_GameState->Player2Money;
    
        if (App::IsKeyPressed('1') && m_Selected == NULL_ENTITY && playerMoney - 50 >= 0)
        {
            Entity entity = ECS.CreateEntity();
            auto transform = Transform(Vec3(m_Cursor->X, m_Cursor->Y, 0.0f), Quat(Vec3(0.0f, 0.0f, 1.0f), 0));
            ECS.AddComponent<Transform>(entity, transform);
            auto rigidbody = RigidBody(1.0, 10.0);
            rigidbody.Color = Vec3(1.0, 0.0, 0.0);
            rigidbody.Category = Obstacle;
            rigidbody.Collidable = true;
            rigidbody.StaticFriction = 1.0f;
            rigidbody.DynamicFriction = 1.0f;
            ECS.AddComponent<RigidBody>(entity, rigidbody);
            ECS.AddComponent<Mesh>(entity, Mesh(WoodCube));
            m_Selected = entity;
            playerMoney -= 50;
            App::PlaySound("Assets/Sounds/chaChingSound.wav");
        }
        else if (App::IsKeyPressed('2') && m_Selected == NULL_ENTITY && playerMoney - 100 >= 0)
        {
            Entity entity = ECS.CreateEntity();
            auto transform = Transform(Vec3(m_Cursor->X, m_Cursor->Y, 0.0f), Quat(Vec3(0.0f, 0.0f, 1.0f), 0));
            ECS.AddComponent<Transform>(entity, transform);
            // metal is 10x heavier than wood
            auto rigidbody = RigidBody(1.0, 10.0, 10.0f);
            rigidbody.Color = Vec3(1.0, 0.0, 0.0);
            rigidbody.Category = Obstacle;
            rigidbody.Collidable = true;
            rigidbody.StaticFriction = 1.0f;
            rigidbody.DynamicFriction = 1.0f;
            ECS.AddComponent<RigidBody>(entity, rigidbody);
            ECS.AddComponent<Mesh>(entity, Mesh(MetalCube));
            m_Selected = entity;
            playerMoney -= 100;
            App::PlaySound("Assets/Sounds/chaChingSound.wav");
        }
        else if (App::IsKeyPressed('2') || App::IsKeyPressed('1'))
        {
            App::PlaySound("Assets/Sounds/wrongSound.wav");
        }
    
    
        if (App::IsKeyPressed('3') && playerMoney - 50 >= 0)
        {
            playerMoney -= 50;
            if (m_GameState->Turn == Player1Turn)
                m_GameState->Player1Inventory.regularCow += 1;
    
            if (m_GameState->Turn == Player2Turn)
                m_GameState->Player2Inventory.regularCow += 1;
    
            App::PlaySound("Assets/Sounds/chaChingSound.wav");
        }
        else if (App::IsKeyPressed('4') && playerMoney - 200 >= 0)
        {
            playerMoney -= 200;
            if (m_GameState->Turn == Player1Turn)
                m_GameState->Player1Inventory.explosiveCow += 1;
    
            if (m_GameState->Turn == Player2Turn)
                m_GameState->Player2Inventory.explosiveCow += 1;
    
            App::PlaySound("Assets/Sounds/chaChingSound.wav");
        }
        else if (App::IsKeyPressed('3') || App::IsKeyPressed('4'))
        {
            App::PlaySound("Assets/Sounds/wrongSound.wav");
        }
    
        if (m_GameState->Turn == Player1Turn)
        {
            m_GameState->Player1Money = playerMoney;
        }
        else if (m_GameState->Turn == Player2Turn)
        {
            m_GameState->Player2Money = playerMoney;
        }
    }

}

void BuildController::HandleSelect()
{
    if (m_Selected != NULL_ENTITY)
    {
        Transform& t = ECS.GetComponent<Transform>(m_Selected);
        RigidBody& rb = ECS.GetComponent<RigidBody>(m_Selected);
        Mesh& mesh = ECS.GetComponent<Mesh>(m_Selected);
        bool hasShader = ECS.HasComponent<Shader>(m_Selected);
        if (!hasShader)
        {
            ECS.AddComponent<Shader>(m_Selected, Shader());
        }
        Shader& shader = ECS.GetComponent<Shader>(m_Selected);
    
        rb.Collidable = false;
        rb.Position = { m_Cursor->X, m_Cursor->Y };
        t.SetPosition2D({ m_Cursor->X, m_Cursor->Y });
    
        if (rb.IsIntersecting)
        {
            shader.ShaderId = RedShader;
        }
        else
        {
            shader.ShaderId = DefaultShader;
        }
    
        if (App::IsKeyPressed('R') && m_RotateAccumulate > 0.1f && mesh.MeshType != SpotFront)
        {
            m_RotateAccumulate = 0.0f;
            float roll, pitch, yaw;
            t.Rotation.GetEulerAngles(roll, pitch, yaw);
            float degreesYaw = 180.0f / 3.141f * yaw;
            if (std::abs(degreesYaw) < 0.1)
            {
                float degrees = 90.0f * 3.141f / 180.0f;
                t = Transform(t.Position, Quat(Vec3(0, 0, 1), degrees));
                rb.Angular = degrees;
            }
            else
            {
                t = Transform(t.Position, Quat(Vec3(0, 0, 1), 0.0));
                rb.Angular = 0.0;
            }
        }
    }
}


