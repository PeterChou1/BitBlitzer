//---------------------------------------------------------------------------------
// RigidBody.h
//---------------------------------------------------------------------------------
//
// Represent a physics body in the physics System a rigid body can be a circle
// or any concave polygon
//
#pragma once


#include <vector>

#include "Vec2.h"
#include "Vec3.h"
#include "Shape.h"
#include "Transform.h"
#include "AABB.h"
#include "ColliderCategory.h"
#include "ECSManager.h"

class Collider;

class RigidBody
{
public:

    RigidBody() = default;

    /**
     * \brief 
     * \param radius 
     */
    RigidBody(float radius);

    /**
     * \brief Construct A Rectangle RigidBody Based on width and height
     * \param width 
     * \param height
     * \param weightMultiplier multiplies rectangle weights
     */
    RigidBody(float width, float height, float weightMultiplier = 1.0f);

    /**
     * \brief Construct a RigidBody based on 
     * \param polygons 
     */
    RigidBody(std::vector<Vec2> polygons);

    /**
     * \brief Sets a collider callback 
     * \param collider 
     */
    void SetCollider(std::shared_ptr<Collider> collider);

    /**
     * \brief Setting the body to infinite mass
     */
    void SetStatic();

    void SyncTransform(Transform& transform);

    void ForwardTransform(Transform& transform) const;

    void RecomputeAABB();

    void ApplyImpulse(const Vec2& impulse);

    void ApplyImpulseAngular(const Vec2& impulse, const Vec2& contactVector);

    void IntegrateVelocity(float deltaTime);

    void IntegrateVelocityAngular(float deltaTime);

    float InvMass() const;

    float InvInertia() const;

    float Restitution() const;

    // specify if rigid body is intersecting with any other rigidbody
    bool IsIntersecting = false;
    // use to sync rigid body with their transform
    bool Initialized = false;
    // specify if a rigid body is collidable
    bool Collidable = true;
    ColliderCategory Category = Default;
    // used for debugging
    Vec3 Color{};
    // mutable states
    AABB RigidBodyAABB{};
    Shape Shape{};
    Vec2 Position{};
    Vec2 Velocity{};
    Vec2 Force{};
    float Angular{};
    float AngularVelocity{};
    float AngularDelta{};
    float StaticFriction{};
    float DynamicFriction{};

private:
    // immutable states
    float m_InvInertia{};
    float m_InvMass{};
    float m_Restitution{};

    static constexpr float DEFAULT_STATIC_FRICTION = 0.6f;
    static constexpr float DEFAULT_DYNAMIC_FRICTION = 0.4f;
    static constexpr float DEFAULT_DENSITY = 1.0f;
    static constexpr float DEFAULT_RESTITUTION = 0.0f;

};


/**
 * \brief A Collider Provides Custom Hooks for developers to interact
 *        with rigid bodies during collision
 */
class Collider
{
public:
    Collider(ColliderCategory A, ColliderCategory B) : m_Pair({A, B}) {}

    virtual ~Collider() = default;

    /**
     * \brief Called in the middle of collision
     */
    virtual void OnCollide(
        Entity self,
        Entity other,
        RigidBody& selfRB,
        RigidBody& otherRB
    ) = 0;


    /**
     * \brief Called on initial collide
     */
    virtual void OnCollideEnter(
        Entity self,
        Entity other,
        RigidBody& selfRB,
        RigidBody& otherRB
    ) = 0;


    /**
     * \brief Called on first frame of collision separation
     */
    virtual void OnCollideExit(
        Entity self,
        Entity other,
        RigidBody& selfRB,
        RigidBody& otherRB
    ) = 0;

    CollisionPair GetCollisionPair() const { return m_Pair; }

private:
    CollisionPair m_Pair;
};