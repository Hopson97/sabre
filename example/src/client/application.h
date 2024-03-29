#pragma once

#include "input/keyboard.h"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>

#include <SFML/Network/Packet.hpp>

#include <sabre/client.h>

struct receivedCommandInfo;

class Application {
  public:
    Application(std::string name = "None");

    void run();

  private:
    constexpr static float PLAYER_HEIGHT = 32;
    constexpr static float PLAYER_WIDTH = 32;

    struct Player {
        Player() { sprite.setSize({PLAYER_WIDTH, PLAYER_HEIGHT}); }

        std::string name;

        sf::RectangleShape sprite;
        sf::Text text;
        sf::Vector2f velocity;

        sf::Vector2f nextPosition;
        float lerpValue;

        bool isConnected = false;
    };

    struct Ball {
        Ball() { sprite.setSize({PLAYER_WIDTH / 2, PLAYER_HEIGHT / 2}); }

        sf::RectangleShape sprite;
        sf::Vector2f nextPosition;
        float lerpValue;
    };

    void input();
    void update(sf::Time delta);
    void render();

    void pollWindowEvents();

    void handlePlayerPosition(Player &player, sf::Packet &packet);
    void handlePlayerName(Player &player, sf::Packet &packet);
    void handleBallPosition(sf::Packet &packet);

    sabre::Client m_client;

    Player &m_player;
    std::array<Player, 4> m_players;
    Ball m_ball;

    sf::RenderWindow m_window;
    sf::Font m_font;
    Keyboard m_keyboard;
};