#include "application.h"

#include <SFML/Window/Event.hpp>
#include <iostream>
#include <sabre/packet_factory.h>

#include "../common/commands.h"
#include "../common/step_timer.h"

Application::Application(std::string name)
    : m_client(sf::IpAddress::LocalHost, 54321,
               [this](const sabre::Event::Details &details) {
                   std::cout << details.senderIp.toString() << std::endl;
                   std::cout << "Client Connected!" << (int)details.id << std::endl;
               },
               [this](const sabre::Event::Details &details) {
                   std::cout << details.senderIp.toString();
               })
    , m_player(m_players[m_client.getClientId()])
{
    m_player.sprite.setPosition(0, 0);
    m_player.isConnected = true;

    m_player.sprite.setOutlineColor(sf::Color::Red);
    m_player.sprite.setOutlineThickness(2);

    m_font.loadFromFile("res/arcade.TTF");

    std::cout << "Client set up! ID: " << (int)m_client.getClientId() << '\n';

    auto nameSetPacket =
        sabre::makePacket(static_cast<sabre::ClientId>(m_client.getClientId()),
                          Command::SetPlayerName);
    nameSetPacket << name;
    m_client.send(nameSetPacket);
}

void Application::run()
{
    if (!m_client.connected()) {
        return;
    }
    m_window.create({WINDOW_WIDTH, WINDOW_HEIGHT}, "UDP Socket playground");
    m_window.setFramerateLimit(60);
    m_window.setKeyRepeatEnabled(false);

    sf::Clock netTimer;

    StepTimer timer(60.f);

    while (m_window.isOpen()) {
        timer.update();

        input();

        timer.whileUpdate([this, &timer] { update(timer.getDelta()); });

        m_client.whileTicking<Command>(
            [this](const sabre::Event::Details &details, sf::Packet &packet,
                   Command command) {
                auto &player =
                    m_players[static_cast<sabre::ClientId>(details.id)];
                player.isConnected = true;
                switch (command) {
                    case Command::PlayerPosition:
                        handlePlayerPosition(player, packet);
                        break;

                    case Command::SetPlayerName:
                        handlePlayerName(player, packet);
                        break;

                    case Command::BallPosition:
                        handleBallPosition(packet);
                        break;

                    default:
                        break;
                }
            });

        if (netTimer.getElapsedTime().asMilliseconds() > 25) {
            auto packet = sabre::makePacket(m_client.getClientId(),
                                            Command::PlayerPosition);
            packet << m_player.sprite.getPosition().x
                   << m_player.sprite.getPosition().y;
            m_client.send(packet);

            netTimer.restart();
        }
        render();
    }
}

void Application::input()
{
    pollWindowEvents();

    // Input
    float speed = 1.0f;
    if (m_keyboard.isKeyDown(sf::Keyboard::Up)) {
        m_player.velocity.y += -speed;
    }
    else if (m_keyboard.isKeyDown(sf::Keyboard::Down)) {
        m_player.velocity.y += speed;
    }
    if (m_keyboard.isKeyDown(sf::Keyboard::Left)) {
        m_player.velocity.x += -speed;
    }
    else if (m_keyboard.isKeyDown(sf::Keyboard::Right)) {
        m_player.velocity.x += speed;
    }
}

void Application::update(sf::Time delta)
{
    m_player.sprite.move(m_player.velocity);
    m_player.velocity *= 0.85f;

    const float x = m_player.sprite.getPosition().x;
    const float y = m_player.sprite.getPosition().y;

    if (x + PLAYER_WIDTH > WINDOW_WIDTH) {
        m_player.sprite.setPosition(WINDOW_WIDTH - PLAYER_WIDTH - 1, y);
    }
    if (y + PLAYER_HEIGHT > WINDOW_HEIGHT) {
        m_player.sprite.setPosition(x, WINDOW_HEIGHT - PLAYER_HEIGHT - 1);
    }
    if (x < 0) {
        m_player.sprite.setPosition(1, y);
    }
    if (y < 0) {
        m_player.sprite.setPosition(x, 1);
    }

    auto lerp = [](float a, float b, float t) { return (1 - t) * a + t * b; };

    // @TODO Handle repeated code here
    for (auto &player : m_players) {
        if (&player == &m_player)
            continue;
        player.lerpValue += delta.asSeconds();
        auto newX = lerp(player.sprite.getPosition().x, player.nextPosition.x,
                         player.lerpValue);
        auto newY = lerp(player.sprite.getPosition().y, player.nextPosition.y,
                         player.lerpValue);

        player.sprite.setPosition(newX, newY);
    }

    m_ball.lerpValue += delta.asSeconds();
    auto newX = lerp(m_ball.sprite.getPosition().x, m_ball.nextPosition.x,
                     m_ball.lerpValue);
    auto newY = lerp(m_ball.sprite.getPosition().y, m_ball.nextPosition.y,
                     m_ball.lerpValue);

    m_ball.sprite.setPosition(newX, newY);
}

void Application::render()
{

    m_window.clear();

    m_window.draw(m_ball.sprite);
    for (auto &player : m_players) {
        if (player.isConnected) {
            player.text.setPosition(player.sprite.getPosition());
            m_window.draw(player.sprite);
            m_window.draw(player.text);
        }
    }

    m_window.display();
}

void Application::pollWindowEvents()
{
    sf::Event e;
    while (m_window.pollEvent(e)) {
        m_keyboard.update(e);
        switch (e.type) {
            case sf::Event::Closed:
                m_window.close();
                break;

            default:
                break;
        }
    }
}

void Application::handlePlayerPosition(Application::Player &player,
                                       sf::Packet &packet)
{
    packet >> player.nextPosition.x >> player.nextPosition.y;
    player.lerpValue = 0;
}

void Application::handlePlayerName(Application::Player &player,
                                   sf::Packet &packet)
{
    packet >> player.name;
    std::cout << "Name: " << player.name << std::endl;

    player.text.setFont(m_font);
    player.text.setOutlineColor(sf::Color::Black);
    player.text.setOutlineThickness(2);
    player.text.setString(player.name);
}

void Application::handleBallPosition(sf::Packet &packet)
{
    packet >> m_ball.nextPosition.x >> m_ball.nextPosition.y;
    m_ball.lerpValue = 0;
}