#include "server.h"

#include <SFML/Network/Packet.hpp>

#include <ctime>
#include <iostream>
#include <random>
#include <thread>

#include <sabre/event.h>
#include <sabre/packet_factory.h>

#include "../common/commands.h"
#include "../common/step_timer.h"

Server::Server()
    : m_server(4u,
               [this](const sabre::Event::Details &details) {
                   auto &player =
                       m_players[static_cast<std::size_t>(details.id)];
                   player.connected = true;
                   player.id = details.id;
                   std::cout << "Player connected!\n";
                   std::cout << "ID: " << (int)player.id << std::endl;
               },
               [this](const sabre::Event::Details &details) {
                   const auto id = details.id;
                   m_players[static_cast<std::size_t>(id)].connected = false;
               })
{
}

void Server::run()
{
    std::mt19937 random;
    random.seed(std::time(nullptr));

    std::uniform_real_distribution<float> dist(-1.f, 1.f);
    m_ball.velocity.x = dist(random) * 3;
    m_ball.velocity.y = dist(random) * 3;

    StepTimer timer(15.0f);
    while (m_isRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        timer.update();

        m_ball.rect.left += m_ball.velocity.x + dist(random);
        m_ball.rect.top += m_ball.velocity.y + dist(random);

        if (m_ball.rect.left < 0 || m_ball.rect.left > WINDOW_WIDTH) {
            m_ball.velocity.x *= -1;
        }
        if (m_ball.rect.top < 0 || m_ball.rect.top > WINDOW_HEIGHT) {
            m_ball.velocity.y *= -1;
        }

        m_server.whileTicking<Command>(
            [this](const sabre::Event::Details &details, sf::Packet &packet,
                   Command command) {
                switch (command) {
                    case Command::PlayerPosition:
                        handlePlayerPosition(details.id, packet);
                        break;

                    case Command::SetPlayerName:
                        handlePlayerNameSet(details.id, packet);
                        break;

                    default:
                        break;
                }
            });

        timer.whileUpdate([this]() {
            for (const auto &player : m_players) {
                if (player.connected) {
                    auto packet =
                        sabre::makePacket(player.id, Command::PlayerPosition);
                    packet << player.rect.left << player.rect.top;
                    m_server.broadcastToPeers(packet);
                }
            }
            auto packet = sabre::makePacket(0, Command::BallPosition);
            packet << m_ball.rect.left << m_ball.rect.top;
            m_server.broadcastToPeers(packet);
        });
    }
}

void Server::handlePlayerPosition(sabre::ClientId id, sf::Packet &packet)
{
    auto &player = m_players[static_cast<std::size_t>(id)];
    float x;
    float y;
    packet >> x >> y;
    player.rect.left = x;
    player.rect.top = y;
}

void Server::handleRequestPlayerPositions(sabre::ClientId requesterId)
{
    for (const auto &player : m_players) {
        if (player.connected && (requesterId != player.id)) {
            auto packet = sabre::makePacket(player.id, Command::PlayerPosition);
            packet << player.rect.left << player.rect.top;
            m_server.sendPacketToPeer(requesterId, packet);
        }
    }
}

void Server::handlePlayerNameSet(sabre::ClientId id, sf::Packet &packet)
{
    auto &player = m_players[static_cast<std::size_t>(id)];
    packet >> player.name;

    std::cout << "Player name set: " << player.name << "\n";

    auto broadcast = sabre::makePacket(player.id, Command::SetPlayerName);
    broadcast << player.name;
    m_server.broadcastToPeers(broadcast);
}