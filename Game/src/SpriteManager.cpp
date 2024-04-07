#include "stdafx.h"

#include <numeric>

#include "SpriteManager.h"
#include "Camera.h"
#include "ECSManager.h"
#include "../App/app.h"
#include "../App/SimpleSprite.h"

extern ECSManager ECS;

SpriteManager::SpriteManager()
{
    m_Speed = 1.0f / 15.0f;
}

void SpriteManager::CreateAnimation(int id, std::string filename, int column, int row, unsigned int count)
{
    CSimpleSprite* sprite = App::CreateSprite(filename.c_str(), column, row);
    sprite->SetScale(1.0f);
    std::vector<int> frames;
    frames.resize(count);
    std::iota(frames.begin(), frames.end(), 0);
    sprite->CreateAnimation(id, m_Speed, frames);
    m_Sprites[id] = sprite;
    m_SpriteAnimCount[id] = count;
    m_Durations[id] = m_Speed * count;
}

void SpriteManager::PlayAnimation(int id, float scale, float x, float y)
{
    std::shared_ptr<Camera> cam = ECS.GetResource<Camera>();


    Vec3 worldPoint = Vec3(x, y, 0.0f);
    Vec2 screenPoint = ECS.GetResource<Camera>()->WorldPointToScreenSpace(worldPoint);

    CSimpleSprite* sprite = new CSimpleSprite("", 0, 0);
    *sprite = *m_Sprites[id];
    sprite->SetAnimation(id);
    sprite->SetPosition(screenPoint.X, screenPoint.Y);
    sprite->SetScale(scale);

    m_WorldPoints[{x, y, id}] = worldPoint;
    m_ActiveSprites[{x, y, id}] = sprite;
    m_ActiveDuration[{x, y, id}] = 0.0f;

}

void SpriteManager::Update(float deltaTime)
{

    std::vector<ActiveSpriteID> deletedSprites;
    std::shared_ptr<Camera> cam = ECS.GetResource<Camera>();

    for (auto& activeSprite : m_ActiveSprites)
    {
        auto& id = activeSprite.first;
        CSimpleSprite* sprite = activeSprite.second;
        m_ActiveDuration[id] += deltaTime / 1000.0f;
        Vec3 worldPoint = m_WorldPoints[id];
        Vec2 screenPoint = ECS.GetResource<Camera>()->WorldPointToScreenSpace(worldPoint);
        sprite->Update(deltaTime);
        sprite->SetPosition(screenPoint.X, screenPoint.Y);

        if (m_ActiveDuration[id] > m_Durations[id.ID] || 
            0 > screenPoint.X || screenPoint.X > APP_VIRTUAL_WIDTH ||
            0 > screenPoint.Y || screenPoint.Y > APP_VIRTUAL_HEIGHT)
        {
            deletedSprites.push_back(id);
        }
    }

    for (auto& deleted : deletedSprites)
    {
        delete m_ActiveSprites[deleted];
        m_WorldPoints.erase(deleted);
        m_ActiveDuration.erase(deleted);
        m_ActiveSprites.erase(deleted);
    }
}

void SpriteManager::Render()
{
    for (auto& activeSprite : m_ActiveSprites)
    {
        activeSprite.second->Draw();
    }

}

void SpriteManager::ResetResource()
{
    for (const auto& activeSprite : m_ActiveSprites)
    {
        delete activeSprite.second;
    }
    m_ActiveDuration.clear();
    m_Durations.clear();
    m_ActiveSprites.clear();
    m_Sprites.clear();
    m_SpriteAnimCount.clear();

}


