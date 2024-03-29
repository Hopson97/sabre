#pragma once

namespace sf {
    class Packet;
    class UdpSocket;
} // namespace sf

#include <SFML/Network/IpAddress.hpp>
#include <functional>

#include "defines.h"

namespace sabre {
    /**
     * @brief An 'Event' sent via UDP sockets, holding information about the
     * event type and the sender
     */
    struct Event final {

        /**
         * @brief The different event types
         */
        enum class EventType : Event_t {
            Connect,
            Data,
            Disconnect,
            KeepAlive,

            RejectConnection,
            AcceptConnection,

            NumConnections,
        };

        /**
         * @brief Info about the sender of the event
         */
        struct Details {
            sf::IpAddress senderIp;
            Port senderPort;
            ClientId id;
        };

        /**
         * @brief Sends a quick response back to the sender of the net event
         *
         * @param socket The socket to send the response with
         * @param type The response type
         */
        void respond(sf::UdpSocket &socket, EventType type) const;
        void respond(sf::UdpSocket &socket, EventType type, ClientId id) const;
        void respond(sf::UdpSocket &socket, sf::Packet& packet) const;

        friend sf::Packet &operator<<(sf::Packet &packet, EventType type);
        friend sf::Packet &operator>>(sf::Packet &packet, EventType &type);

        EventType type;
        Details details;
    }; // struct Event

    /**
     * @brief Receives an event via a UDP socket
     *
     * @param socket The socket to receive the event on
     * @param packet The packet that was received
     * @param event The event that was received
     * @return true An event has been received
     * @return false Nothing was received
     */
    bool receiveNetEvent(sf::UdpSocket &socket, sf::Packet &packet,
                         Event &event);

    using OnEventFunction = std::function<void(const Event::Details &details)>;
} // namespace sabre
