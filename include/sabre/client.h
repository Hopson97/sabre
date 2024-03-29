#pragma once

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/System/Clock.hpp>
#include <array>
#include <functional>
#include <iostream>

#include "event.h"
#include "packet_factory.h"

namespace sabre {
    class Client final {
      public:
        Client(const sf::IpAddress &serverAddress, Port serverPort,
               OnEventFunction onPeerConnect, OnEventFunction onPeerDisconnect);

        ~Client();

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

        bool send(sf::Packet &packet);
        bool connected() const;

        ClientId getClientId() const;

      private:
        void handlePeerConnection(const Event &event);

        sf::UdpSocket m_socket;

        const sf::IpAddress m_serverAddress;
        const Port m_serverPort;
        ClientId m_id;

        OnEventFunction m_onPeerConnect;
        OnEventFunction m_onPeerDisconnect;

        bool m_isConnected = false;

        // Used for receiving packets
        sf::IpAddress m_receivedIp;
        sf::Clock m_stayAliveClock;
        Port m_receivedPort;
    };

    template <typename CommandEnum, typename Callback>
    bool Client::whileTicking(Callback callback)
    {
        if (m_stayAliveClock.getElapsedTime() > sf::seconds(2)) {
            m_stayAliveClock.restart();
            auto packet = makePacket(Event::EventType::KeepAlive);
            send(packet);
        }
        Event event;
        sf::Packet packet;
        CommandEnum command;
        while (receiveNetEvent(m_socket, packet, event)) {
            switch (event.type) {
                case Event::EventType::Connect:
                    handlePeerConnection(event);
                    break;

                case Event::EventType::Disconnect:
                    // handle disconnect...
                    m_onPeerDisconnect(event.details);
                    break;

                case Event::EventType::Data:
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
