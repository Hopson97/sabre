#include <sabre/event.h>

#include <SFML/Network/Packet.hpp>
#include <SFML/Network/UdpSocket.hpp>

#include <iostream>

#include <sabre/packet_factory.h>

namespace {
    void respond(sf::UdpSocket &socket, const sabre::Event::Details &details,
                 sf::Packet &packet)
    {
        if (socket.send(packet, details.senderIp, details.senderPort) !=
            sf::Socket::Done) {
            std::cerr << "Failed to send response packet\n";
        }
    }
} // namespace

namespace sabre {
    void Event::respond(sf::UdpSocket &socket, EventType type) const
    {
        auto packet = makePacket(type);
        ::respond(socket, details, packet);
    }

    void Event::respond(sf::UdpSocket &socket, EventType type,
                        ClientId id) const
    {
        auto packet = makePacket(type, id);
        ::respond(socket, details, packet);
    }

    void Event::respond(sf::UdpSocket &socket, sf::Packet &packet) const
    {
        ::respond(socket, details, packet);
    }

    sf::Packet &operator<<(sf::Packet &packet, Event::EventType type)
    {
        packet << static_cast<Event_t>(type);
        return packet;
    }

    sf::Packet &operator>>(sf::Packet &packet, Event::EventType &type)
    {
        Event_t commandId;
        packet >> commandId;
        type = static_cast<Event::EventType>(commandId);
        return packet;
    }

    bool receiveNetEvent(sf::UdpSocket &socket, sf::Packet &packet,
                         Event &event)
    {
        if (socket.receive(packet, event.details.senderIp,
                           event.details.senderPort) != sf::Socket::Done) {
            return false;
        }
        packet >> event.type >> event.details.id;
        return true;
    }
} // namespace sabre