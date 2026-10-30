#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iomanip>
#include <sstream>

#include "game.h"
#include "utils.h"

int main() {
  bool is_observer = false;
  unsigned short observer_port = 53000;
  unsigned short playerPort = sf::Socket::AnyPort;

  Utils::printMsg("Game startup...");

  std::string title_type = "";

  bool ready = false;

  while (!ready) {
    Utils::printMsg(
        "Are we playing or observing? Enter 1 for Player, enter 2 for "
        "Observer:");
    std::string input_line;
    std::getline(std::cin, input_line);

    if (!input_line.empty() && input_line.at(0) == '1') {
      Utils::printMsg("Player chosen, preparing window...");
      title_type = "Player";
      is_observer = false;
      ready = true;
    } else if (!input_line.empty() && input_line.at(0) == '2') {
      Utils::printMsg("Observer chosen, preparing window...");
      title_type = "Observer";
      is_observer = true;
      ready = true;
    } else {
      Utils::printMsg("Incorrect input, please try again!", warning);
    }
  }

  // Prepare window.
  sf::RenderWindow window(
      sf::VideoMode({640, 480}),
      "CMP425/CMP501 - Lab 3: Packets (" + title_type + ")");
  window.setFramerateLimit(60);  // Request 60 frames per second
  Utils::printMsg("Window ready, configuring networking...");

  // Prepare networking.
  sf::UdpSocket socket;
  auto port = is_observer ? observer_port : playerPort;

  Utils::printMsg("Binding UDP socket...");
  if (socket.bind(port) != sf::Socket::Status::Done) {
    Utils::printMsg("Error binding socket!", error);
  } else {
    Utils::printMsg("Socket bound to port: " +
                    std::to_string(socket.getLocalPort()));
  }

  // Clock for timing the 'dt' value
  sf::Clock clock;

  // Game object.
  Game game;

  // Other game parameters.
  float game_speed = 1.0f;
  float send_rate = 0.1f;
  float send_timer = 0;

  while (window.isOpen()) {
    // Calculate dt.
    float dt = clock.restart().asSeconds() * game_speed;
    send_timer += dt;  // Count how much time has passed.
    // Handle window events (e.g. key press).
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        Utils::printMsg("Window closed, unbinding socket...",
                        MessageType::warning);
        window.close();
        socket.unbind();
      }
      if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
          Utils::printMsg("ESC pressed, closing window, unbinding socket...",
                          MessageType::warning);
          window.close();
          socket.unbind();
        }
      }
      // We only handle inputs for the player. Observer will do direct updates
      // from network data.
      if (!is_observer) game.HandleEvents(event);
    }
    // Packet to hold our updates.
    sf::Packet packet;
    if (is_observer) {
      std::optional<sf::IpAddress> incoming_ip;
      unsigned short incoming_port;

      // FIXME: Recieve can fail, check for errors.
      if (socket.receive(packet, incoming_ip, incoming_port) ==
          sf::Socket::Status::Done) {
        Utils::printMsg(
            "Recieved message from: " + incoming_ip.value().toString() + ":" +
            std::to_string(incoming_port));
        TankMessage message;

        // Read recieved data into TankMessage struct.
        // FIXME: reading from packet can fail. Refer to documentation on how to
        // handle errors and add error checking here.
        packet >> message.x >> message.y;
        // Use the message data to update the game.
        game.NetworkUpdate(dt, message);
      } else {
        Utils::printMsg("Failed to recieve message from: " +
                            incoming_ip.value().toString() + ":" +
                            std::to_string(incoming_port),
                        error);
      }
    } else {
      game.Update(dt);
      // Get data structure from game, containing update message.
      TankMessage message = game.GetNetworkUpdate();
      // Translate our messgage struct to sf::Packet (very rudimentary
      // conversion)
      packet << message.x << message.y;
      sf::IpAddress observerIp(127, 0, 0, 1);

      // Send messages only as often as the send rate allows.
      if (send_timer >= send_rate) {
        // FIXME: Send can fail, check for errors and adjust logic accordingly.
        if (socket.send(packet, observerIp, observer_port) ==
            sf::Socket::Status::Done) {
          Utils::printMsg("Sent message to: " + observerIp.toString() + ":" +
                          std::to_string(observer_port));
        } else {
          Utils::printMsg(
              "Failed to send message to: " + observerIp.toString() + ":" +
                  std::to_string(observer_port),
              error);
        }
        // Reset timer after sending the message
        send_timer = 0;
      }
    }
    // Render
    window.clear();
    game.Render(window);  // This takes window as a reference.
    window.display();
  }

  return 0;
}
