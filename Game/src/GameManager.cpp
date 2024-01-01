#include "stdafx.h"
#include "../App/AppSettings.h"
#include "GameManager.h"
#include "Utils.h"
#include "Camera.h"
#include "SimpleTexture.h"
#include "DepthBuffer.h"
#include "GraphicsBuffer.h"

extern Coordinator gCoordinator;
constexpr double ASPECT_RATIO = APP_VIRTUAL_WIDTH / APP_VIRTUAL_HEIGHT;

void Scene::Setup()
{
    // Setup Camera
    Entity camEntity = gCoordinator.CreateEntity();
    auto cam = Camera(Vec3(0, 0, 0), Vec3(0, 0, 1), Vec3(0, 1, 0), APP_VIRTUAL_WIDTH, APP_VIRTUAL_HEIGHT, 90.0f,
                      ASPECT_RATIO, 0.1, 100);


    // Setup Rendering
    Entity dBufferEntity = gCoordinator.CreateEntity();
    auto depthBuffer = DepthBuffer(APP_VIRTUAL_WIDTH, APP_VIRTUAL_HEIGHT);
    Entity simdDepthEntity = gCoordinator.CreateEntity();
    auto simdDepth = DepthBufferSIMD(APP_VIRTUAL_WIDTH, APP_VIRTUAL_HEIGHT);

    Entity graphicsEntity = gCoordinator.CreateEntity();
    auto g = GraphicsBuffer(APP_VIRTUAL_WIDTH, APP_VIRTUAL_HEIGHT);

    Entity colorEntity = gCoordinator.CreateEntity();
    auto color = ColorBuffer(APP_VIRTUAL_WIDTH, APP_VIRTUAL_HEIGHT);

    Entity textureEntity = gCoordinator.CreateEntity();
    auto texList = TextureList();

    Entity simdPixelEntity = gCoordinator.CreateEntity();
    auto simdPixelBuffer = SIMDPixelBuffer(APP_VIRTUAL_WIDTH, APP_VIRTUAL_HEIGHT);


    // Setup Mesh Instance
    MeshInstance meshI;
    bool loadsucceed = Utils::LoadInstance("./Assets/big.obj", meshI, texList);
    assert(loadsucceed && "failed to load file");

    for (int x = 0; x < 1; x++)
    {
        for (int z = 0; z < 1; z++)
        {
            Entity testRenderEntity = gCoordinator.CreateEntity();
            auto modelTransform = Transform(Vec3(x * 2, 0, 5 + z * 2), Quat(Vec3(1, 0, 0), 0.0)); // 3.14 / 2));
            MeshInstance meshCopy = meshI;
            gCoordinator.AddComponent<Transform>(testRenderEntity, modelTransform);

            for (auto& vert : meshCopy.vertices)
            {
                vert.pos = modelTransform.TransformVec3(vert.pos);
                vert.normal = modelTransform.TransformNormal(vert.normal);
            }

            g.AddMeshInstance(meshCopy);

            // Entity e = gCoordinator.CreateEntity();
            // auto mesh_obj = Mesh(meshCopy);
            // auto t2 = Transform(Vec3(x * 2, 0, 10), Quat(Vec3(1, 0, 0), 0.0)); // 3.14 / 2));
            // auto texture = SimpleTexture("./Assets/spot_texture.png",
            //                              Vec3(0, 0, 0),
            //                              Vec3(0, 0, 0),
            //                              Vec3(0, 0, 0), 0.0);
            // // 
            // gCoordinator.AddComponent<Mesh>(e, mesh_obj);
            // gCoordinator.AddComponent<Transform>(e, t2);
            // gCoordinator.AddComponent<SimpleTexture>(e, texture);
        }
    }


    gCoordinator.AddComponent<Camera>(camEntity, cam);
    gCoordinator.AddComponent<DepthBuffer>(dBufferEntity, depthBuffer);
    gCoordinator.AddComponent<GraphicsBuffer>(graphicsEntity, g);
    gCoordinator.AddComponent<ColorBuffer>(colorEntity, color);
    gCoordinator.AddComponent<DepthBufferSIMD>(simdDepthEntity, simdDepth);
    gCoordinator.AddComponent<TextureList>(textureEntity, texList);
    gCoordinator.AddComponent<SIMDPixelBuffer>(simdPixelEntity, simdPixelBuffer);


    debugCam = std::make_shared<DebugCamera>(
        DebugCamera(gCoordinator.GetComponent<Camera>(camEntity))
    );
    rendererM = std::make_shared<RendererM>(
        RendererM(
            gCoordinator.GetComponent<GraphicsBuffer>(graphicsEntity),
            gCoordinator.GetComponent<Camera>(camEntity),
            gCoordinator.GetComponent<ColorBuffer>(colorEntity),
            gCoordinator.GetComponent<DepthBufferSIMD>(simdDepthEntity),
            gCoordinator.GetComponent<TextureList>(textureEntity)
        )
    );
    renderer = std::make_shared<Renderer>(
        Renderer(
            gCoordinator.GetComponent<Camera>(camEntity),
            gCoordinator.GetComponent<DepthBuffer>(dBufferEntity),
            gCoordinator.GetComponent<ColorBuffer>(colorEntity)
        )
    );
}

void Scene::Render()
{
    rendererM->Render();
    //renderer->Render();
    debugCam->Render();
}

void Scene::Update(float deltaTime)
{
    debugCam->Move(deltaTime);
}
