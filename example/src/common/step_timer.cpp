#include "step_timer.h"

StepTimer::StepTimer(float ticksPerSecond)
    : m_timePerUpdate(sf::seconds(1.0f / ticksPerSecond))
{
}

void StepTimer::update()
{
    auto time = m_timer.getElapsedTime();
    m_delta = time - m_lastTime;

    m_lastTime = time;
    m_lag += m_delta;
}