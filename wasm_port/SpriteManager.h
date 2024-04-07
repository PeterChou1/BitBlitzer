//---------------------------------------------------------------------------------
// SpriteManager.h
//---------------------------------------------------------------------------------
//
// Manages sprite animation plays 2d sprite animation in our 3d world
//
#pragma once
#include <string>
#include <unordered_map>

#include "Resource.h"
#include "Vec3.h"

class CSimpleSprite;

struct ActiveSpriteID
{
    float X;
    float Y;
    int ID;

    bool operator==(const ActiveSpriteID& other) const
    {
        return X == other.X && Y == other.Y && ID == other.ID;
    }
};

struct ActiveSpriteHash {
    std::size_t operator()(const ActiveSpriteID& k) const
    {
        return ((std::hash<float>()(k.X) ^ (std::hash<float>()(k.Y) << 1)) >> 1) ^ (std::hash<int>()(k.ID) << 1);
    }
};

class SpriteManager : public Resource
{
public:

    SpriteManager();

    void CreateAnimation(
        int id,
        std::string filename, 
        int column, int row, 
        unsigned int count
    );

    void PlayAnimation(int id, float scale, float x, float y);

    void Update(float deltaTime);

    void Render();

    void ResetResource() override;

private:
    float m_Speed;
    std::unordered_map<ActiveSpriteID, float, ActiveSpriteHash> m_ActiveDuration;
    std::unordered_map<ActiveSpriteID, Vec3, ActiveSpriteHash> m_WorldPoints;
    std::unordered_map<int, float> m_Durations;
    std::unordered_map<int, CSimpleSprite*> m_Sprites;
    std::unordered_map<int, unsigned int> m_SpriteAnimCount;
    std::unordered_map<ActiveSpriteID, CSimpleSprite*, ActiveSpriteHash> m_ActiveSprites;
};
