#pragma once

/**
 * \brief Every Tick of the Frame a system will delete
 *        Current Time by deltaTime when it reaches zero the
 *        Entity it is attached to will be deleted
 */
struct Timer
{
    float CurrentTime{};

    Timer() = default;

    Timer(float time) : CurrentTime(time) {}
};