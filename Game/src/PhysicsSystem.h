#pragma once



class PhysicsSystem
{
public:

    void Update(float deltaTime);

    void Step();

private:
    float accumulate{};
    constexpr static float impulseIteration = 6;
};