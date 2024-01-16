#pragma once



class PhysicsSystem
{
public:

    void SyncTransform();

    void ForwardTransform();

    void Update(float deltaTime);

    void Step();

private:

    float m_Accumulate{};

    // How much times the physics iteration is run per time step
    // increase for more accuracy at the cost performance
    constexpr static float STEP_ITERATION = 6;
};