#pragma once

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/System/Clock.hpp>

#include <array>

#include "event.h"

struct Event;

namespace sabre {
    /**
     * @brief The server that clients can connect to
     */
    class Server final {

        /**
         * @brief Each connected client to this server is considered a
         * ConnectedClient
         */
        struct ConnectedClient {
            sf::IpAddress address;
            Port port;
            sf::Time lastUpdate;
        };

      public:
        static constexpr std::size_t MAX_CONNECTIONS = 4;

        Server(std::size_t maxConnections, OnEventFunction onClientConnect,
               OnEventFunction onClientDisconnect);

        /**
         * @brief Handles stream of all the packets being received
         *
         * @tparam CommandEnum
         * @tparam Callback
         * @param callback
         * @return true
         * @return false
         */
        template <typename CommandEnum, typename Callback>
        bool whileTicking(Callback callback);

        void sendPacketToPeer(ClientId peerId, sf::Packet &packet);
        void broadcastToPeers(sf::Packet& packet);

      private:
        void handleIncomingConnection(const Event &event);
        void handleClientDisconnect(const Event& event);
        void handleNumConnections(const Event& event);

        void keepAlive(const Event &event);

        std::size_t emptySlot() const;
        ConnectedClient &getClient(ClientId id);

        uint16_t m_currentConnections = 0;
        const uint16_t m_maxConnections;
        
        std::vector<ConnectedClient> m_clients;
        std::vector<uint8_t>  m_clientConnected;


        sf::UdpSocket m_socket;
        sf::Clock m_clock;
        sf::Clock m_keepAliveClock;
        sf::Time m_clientTimeout = sf::seconds(3.0);

        OnEventFunction m_onConnect;
        OnEventFunction m_onDisconnect;
    };

    template <typename CommandEnum, typename Callback>
    bool Server::whileTicking(Callback callback)
    {
        /*
        if (m_keepAliveClock.getElapsedTime() > m_clientTimeout) {
            for (std::size_t i = 0 ; i < m_clients.size(); i++) {
                if (m_clientConnected[i]) {
                    if (m_clock.getElapsedTime() - m_clients[i].lastUpdate > sf::seconds(m_timeout)) {
                        auto packet = makePacket(Event::Type::Disconnect, static_cast<ClientId>(i));

                        m_onDisconnect(static_cast<ClientId>(i));
                    }
                }
            }
        }*/
        Event event;
        sf::Packet packet;
        CommandEnum command;
        while (receiveNetEvent(m_socket, packet, event)) {
            switch (event.type) {
                case Event::EventType::Connect:
                    handleIncomingConnection(event);
                    break;

                case Event::EventType::Disconnect:
                    // handle disconnect...
                    m_onDisconnect(event.details);
                    break;

                case Event::EventType::KeepAlive:
                    keepAlive(event);
                    break;

                case Event::EventType::NumConnections:
                    handleNumConnections(event);
                    break;


                case Event::EventType::Data:
                    keepAlive(event);
                    packet >> command;
                    callback(event.details, packet, command);
                    break;

                default:
                    break;
            }
            return true;
        }
        return false;
    }
} // namespace sabre