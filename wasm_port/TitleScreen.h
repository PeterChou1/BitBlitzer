#pragma once
#include "Camera.h"
#include "Lighting.h"
#include "Scene.h"


class TitleScreen : public Scene
{
public:

    TitleScreen() = default;

    ~TitleScreen() override = default;

    void Start() override;

    void Setup() override;

    void Update(float deltaTime) override;

    void Render() override;

private:
    float m_Accumulate{};
    std::shared_ptr<Camera> m_Cam;
    std::shared_ptr<Lighting> m_Light;
};
