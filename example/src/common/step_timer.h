#pragma once

#include <SFML/System/Clock.hpp>

class StepTimer {
  public:
    StepTimer(float ticksPerSecond);

    void update();

    template <typename F> void whileUpdate(F f)
    {
        while (m_lag >= m_timePerUpdate) {
            f();
            m_lag -= m_timePerUpdate;
        }
    }

    sf::Time getDelta() const { return m_delta; }

  private:
    sf::Clock m_timer;
    sf::Time m_lastTime;
    sf::Time m_delta;
    sf::Time m_lag;

    const sf::Time m_timePerUpdate;
};