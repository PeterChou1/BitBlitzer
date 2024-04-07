
#include "stdafx.h"

#include <random> 

#include "TitleScreen.h"
#include "AssetServer.h"
#include "ECSManager.h"
#include "GameManager.h"
#include "Mesh.h"
#include "Shader.h"
#include "app.h"

extern ECSManager ECS;
extern GameManager GameSceneManager;

ShaderAsset GetNextShader(ShaderAsset current)
{
    switch (current)
    {
    case DefaultShader:
        return BlinnPhong;
    case BlinnPhong:
        return ToonShader;
    case ToonShader:
        return UnlitShader;
    case UnlitShader:
        return BlinnPhong;
    }

    return BlinnPhong;
}

void SpawnSpot(Vec3 position)
{
    Entity meshEntity = ECS.CreateEntity();
    ECS.AddComponent<Mesh>(meshEntity, Mesh(Spot));
    ECS.AddComponent<Transform>(meshEntity, Transform(position, Quat()));
    ECS.AddComponent<Shader>(meshEntity, Shader(BlinnPhong));
}

void TitleScreen::Start()
{
    m_Cam = ECS.GetResource<Camera>();
    m_Light = ECS.GetResource<Lighting>();
}

void TitleScreen::Setup()
{
    auto& server = AssetServer::GetInstance();
    server.LoadLevelAssets({ Spot });
    m_Cam->SetPositionAndOrientation(
        Vec3(0, 0, 3),
        Vec3(0, 0, 0),
        Vec3(0, 1, 0)
    );

    // Spawn a bunch of cows
    SpawnSpot(Vec3(0, 0, 0));
    SpawnSpot(Vec3(3, 3, -5));
    SpawnSpot(Vec3(-3, 3, -5));
    SpawnSpot(Vec3(-3, -3, -5));
    SpawnSpot(Vec3(3, -3, -5));

}

void TitleScreen::Update(float deltaTime)
{
    float deltaSecond = deltaTime / 1000.0f;
     m_Accumulate += deltaSecond;
    float spinSpeed = 30 * 3.141f / 180.0f * deltaSecond;
    float x, y;
    bool shaderSwitched = false;
    App::GetMousePos(x, y);
    // Convert Mouse Position to Camera
    // Make mouse the light source as demonstration of lighting system
    Vec3 planePoint = Vec3(0, 0, -5);
    Vec3 planeNormal = Vec3(0, 0, -1);
    Vec3 mousePoint = m_Cam->ScreenSpaceToWorldPoint(x, y, planePoint, planeNormal);
    m_Light->SetLight(mousePoint, Vec3(1, 1, 1), 40.0f);

    for (Entity e : ECS.Visit<Mesh, Transform, Shader>())
    {
        Transform& transform = ECS.GetComponent<Transform>(e);
        Shader& shader = ECS.GetComponent<Shader>(e);
    
        // Make Spot spin around the y axis - x axis
        transform.Update(
            Vec3(0, 0, 0),
            Quat(Vec3(0, 1, 0), spinSpeed)
        );
        transform.Update(
            Vec3(0, 0, 0),
            Quat(Vec3(1, 0, 0), spinSpeed / 2)
        );
    
        if (App::IsKeyPressed('Q') && m_Accumulate > 0.1)
        {
            ShaderAsset shaderID = GetNextShader(shader.ShaderId);
            shader.ShaderId = shaderID;
            AssetServer::defaultShader = shaderID;
            shaderSwitched = true;
        }
    }
    // 
    if (shaderSwitched)
    {
        m_Accumulate = 0.0f;
    }
    // pressed space bar to load level1 
    if (App::IsKeyPressed(' '))
    {
        GameSceneManager.SetActiveScene("Level1");
    }
}

void TitleScreen::Render()
{
    float width = APP_VIRTUAL_WIDTH / 2.0f;
    App::Print(width - 50, 700, "ANGRY COWS");
    App::Print(width - 75, 650, "Press Space to Start");
    App::Print(width - 90, 600, "Press Q to switch shaders");
}
