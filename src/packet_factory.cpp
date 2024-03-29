#include <sabre/packet_factory.h>

namespace sabre {
    sf::Packet makePacket(Event::EventType type)
    {
        sf::Packet packet;
        packet << type;
        return packet;
    }

    sf::Packet makePacket(Event::EventType type, ClientId id)
    {
        sf::Packet packet;
        packet << type << id;
        return packet;
    }
} // namespace sabre