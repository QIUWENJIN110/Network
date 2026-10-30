/*	CMP425 / CMP501
    Lab 1 TCP client example - by Andrei Boiko

    A simple client that connects to a server and waits for
    a response. The server sends "hello" when the client first
    connects. Text typed is then sent to the server which echos
    it back, and the response is printed out.
*/

// #include <entt.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include "utils.h"

constexpr auto WELCOME_MSG = "Hello client!";
constexpr auto MESSAGESIZE = 40;
auto serverPort = 53000;
// auto ip = sf::IpAddress::resolve("127.0.0.1"); // more readable option
sf::IpAddress serverIp(127, 0, 0,
                       1);  // safer option as it avoids string parsing

int main() {
  // Initialise the window and draw a simple shape to test that it works.
  sf::RenderWindow window(sf::VideoMode({800, 600}), "SFML Client");
  sf::CircleShape shape(100.f);
  shape.setFillColor(sf::Color::Green);
  shape.setPosition(sf::Vector2f(300, 200));

  Utils::printMsg("Client startup. Connecting to server...");

  // Create a TCP socket that we'll connect to the server.
  sf::TcpSocket socket;

  // Connect the socket to the server.
  sf::Socket::Status status = socket.connect(serverIp, serverPort);
  if (status != sf::Socket::Status::Done) {
    Utils::printMsg("Error connecting to server!", error);
    return 1;  // Exit the program with an error code.
    // FIXME: currently, the application will continue even if it fails to
    // connect. Handle this more gracefully.
  }

  // We'll use this array to hold the messages we exchange with the server.
  char buffer[MESSAGESIZE];
  std::size_t message_size;

  // We expect the server to send us a welcome message (WELCOME_MSG) when we
  // connect.

  // Receive the message.
  sf::Socket::Status receiveStatus =
      socket.receive(buffer, sizeof(buffer), message_size);
  // FIXME: check for errors, and/or for unexpected message size.
  if (receiveStatus != sf::Socket::Status::Done) {
    Utils::printMsg("Error receiving welcome message!", error);
    return 1;  // Exit the program with an error code.
  }

  Utils::printMsg(std::string(buffer).substr(0, MESSAGESIZE), debug);
  // Check it's what we expected.
  if (memcmp(buffer, WELCOME_MSG, strlen(WELCOME_MSG)) != 0) {
    Utils::printMsg("Expected \"" +
                        std::string(WELCOME_MSG).substr(0, MESSAGESIZE) +
                        "\" upon connection, but got something else!",
                    error);
  }

  // Main loop will run while the window is open.
  while (window.isOpen()) {
    // Check for window events.
    while (const std::optional event = window.pollEvent()) {
      // If "close" button is pressed, close the window.
      if (event->is<sf::Event::Closed>()) window.close();
    }

    // Clear frame.
    window.clear();

    // Draw new frame.
    window.draw(shape);

    // Display Frame.
    window.display();

    // -----------------------------------------------------------------------------
    // Normally you want to draw frames after doing the input/logic/networking,
    // but we do it first so that we could check that the window works as
    // expected.
    // -----------------------------------------------------------------------------

    Utils::printMsg("Please enter a message: ");
    // Read a line of text from the user.
    std::string line;
    std::getline(std::cin, line);
    // Now "line" contains what the user typed (without the trailing \n).

    // Copy the line into the buffer, filling the rest with underscores.
    memset(buffer, '~', MESSAGESIZE);
    size_t copySize =
        std::min(line.size(),
                 static_cast<size_t>(
                     MESSAGESIZE - 1));  // Ensure we don't overflow the buffer.
    memcpy(buffer, line.c_str(), copySize);  // Copy the line into the buffer.
    buffer[copySize] = '\0';  // Null-terminate the string in the buffer.
    // FIXME: if line.size() is bigger than the buffer it'll overflow (and
    // likely corrupt memory)
    if (line.size() > MESSAGESIZE - 1) {
      Utils::printMsg("Warning: message is too long and will be truncated.",
                      warning);
    } else if (line.size() == 0) {
      Utils::printMsg("Warning: message is empty.", warning);
    }

    Utils::printMsg("Sending message to server: ");
    Utils::printMsg(std::string(buffer).substr(0, MESSAGESIZE), debug);
    // Send the message to the server.
    // socket.send(buffer, MESSAGESIZE);
    // FIXME: check for errors from send
    sf::Socket::Status sendStatus = socket.send(buffer, MESSAGESIZE);
    if (sendStatus != sf::Socket::Status::Done) {
      Utils::printMsg("Error sending message to server!", error);
      break;
    }

    // Read a response back from the server.
    // socket.receive(buffer, sizeof(buffer), message_size);
    // FIXME: check for errors from recieve
    sf::Socket::Status receiveStatus =
        socket.receive(buffer, sizeof(buffer), message_size);
    if (receiveStatus != sf::Socket::Status::Done) {
      Utils::printMsg("Error receiving message from server!", error);
      break;
    }

    // Print the message we recieved.
    Utils::printMsg("Echo recieved: ", success);
    Utils::printMsg(std::string(buffer).substr(0, MESSAGESIZE), debug);
  }

  socket.disconnect();
}
