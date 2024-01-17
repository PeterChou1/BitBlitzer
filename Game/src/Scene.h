//---------------------------------------------------------------------------------
// Scene.h
//---------------------------------------------------------------------------------
//
// A Scene represents a discrete chunk in the game such as a level or a menu
//
#pragma once

class Scene
{
public:
    Scene() = default;


    void Setup();

    void Update(float deltaTime);

};
