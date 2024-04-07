#pragma once


enum CowType
{
    RegularCow,
    ExplosiveCow
};

struct CowProjectile
{
    CowType ProjectileType;

    CowProjectile(CowType projectileType) : ProjectileType(projectileType) {}

    CowProjectile() : ProjectileType(RegularCow) {}
};