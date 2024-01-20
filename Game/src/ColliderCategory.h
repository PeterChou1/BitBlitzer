#pragma once

#include <utility>

enum ColliderCategory
{
    Default,
    Ground,
    Wall,
    Cow,
    Projectile
};

using CollisionPair = std::pair<ColliderCategory, ColliderCategory>;

struct CollisionPairHash
{
    std::size_t operator()(const CollisionPair& pair) const {
        return std::hash<ColliderCategory>()(pair.first) ^ (std::hash<ColliderCategory>()(pair.second) << 1);
    }
};