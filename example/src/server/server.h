#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/System/Clock.hpp>
#include <sabre/server.h>

#include <array>

struct receivedCommandInfo;

class Server final {
    struct Player {
        sf::FloatRect rect;
        std::string name;

        sabre::ClientId id;
        bool connected = false;
    };

    struct Ball {
        sf::FloatRect rect;
        sf::Vector2f velocity;
    };

  public:
    Server();

    void run();

  private:
    void handlePlayerPosition(sabre::ClientId id, sf::Packet &packet);
    void handleRequestPlayerPositions(sabre::ClientId id);
    void handlePlayerNameSet(sabre::ClientId id, sf::Packet &packet);

    sabre::Server m_server;
    std::array<Player, sabre::Server::MAX_CONNECTIONS> m_players;
    Ball m_ball;

    bool m_isRunning = true;
};