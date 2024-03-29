#include <sabre/event.h>
#include <sabre/server.h>

#include <SFML/Network/Packet.hpp>
#include <iostream>

#include <sabre/packet_factory.h>

namespace sabre {
    Server::Server(std::size_t maxConnections, OnEventFunction onClientConnect,
                   OnEventFunction onClientDisconnect)
        : m_maxConnections(maxConnections)
        , m_clients(static_cast<std::size_t>(maxConnections))
        , m_clientConnected(static_cast<std::size_t>(maxConnections), false)
        , m_onConnect(onClientConnect)
        , m_onDisconnect(onClientDisconnect)
    {
        m_socket.bind(54321);
        m_socket.setBlocking(false);
    }

    void Server::sendPacketToPeer(ClientId peerId, sf::Packet &packet)
    {
        if (m_clientConnected[peerId]) {
            auto &client = getClient(peerId);
            m_socket.send(packet, client.address, client.port);
        }
    }

    void Server::broadcastToPeers(sf::Packet &packet)
    {

        for (std::size_t i = 0; i < m_clients.size(); i++) {
            sendPacketToPeer(static_cast<ClientId>(i), packet);
        }
    }

    void Server::handleIncomingConnection(const Event &event)
    {
        if (auto slot = emptySlot(); slot < MAX_CONNECTIONS) {
            m_clientConnected[slot] = true;
            m_clients[slot].address = event.details.senderIp;
            m_clients[slot].port = event.details.senderPort;
            m_clients[slot].lastUpdate = m_clock.getElapsedTime();

            event.respond(m_socket, Event::EventType::AcceptConnection,
                          static_cast<ClientId>(slot));

            m_onConnect({event.details.senderIp, event.details.senderPort,
                         static_cast<ClientId>(slot)});

            auto packet = makePacket(Event::EventType::Connect,
                                     static_cast<ClientId>(slot));
            broadcastToPeers(packet);
        }
        else {
            event.respond(m_socket, Event::EventType::RejectConnection);
        }
    }

    void Server::handleDisconnect(const Event &event)
    {
        m_onDisconnect({event.details.senderIp, event.details.senderPort, event.details.id});
        m_clientConnected[static_cast<std::size_t>(event.details.id)] = false;

        auto packet =
            makePacket(Event::EventType::Disconnect, event.details.id);
        broadcastToPeers(packet);
    }

    void Server::keepAlive(const Event &event)
    {
        auto &client = getClient(event.details.id);
        client.lastUpdate = m_clock.getElapsedTime();
    }

    std::size_t Server::emptySlot() const
    {
        for (std::size_t i = 0; i < MAX_CONNECTIONS; i++) {
            if (!m_clientConnected[i]) {
                return i;
            }
        }
        return MAX_CONNECTIONS + 1;
    }

    void Server::handleNumConnections(const Event &event)
    {
        auto packet = makePacket(Event::EventType::NumConnections);
        packet << m_currentConnections << m_maxConnections;
        event.respond(m_socket, packet);
    }

    Server::ConnectedClient &Server::getClient(ClientId id)
    {
        return m_clients[static_cast<std::size_t>(id)];
    }

} // namespace sabre
