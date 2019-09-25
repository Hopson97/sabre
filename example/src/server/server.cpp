#include "server.h"

#include <SFML/Network/Packet.hpp>

#include <iostream>
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
    StepTimer timer(15.0f);
    while (m_isRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        timer.update();
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