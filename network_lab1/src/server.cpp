/*	CMP425 / CMP501
    Lab 1 TCP server example - by Andrei Boiko

    A simple TCP server that waits for a connection.
    When a connection is made, the server sends "hello" to the client.
    It then repeats back anything it receives from the client.
    All the calls are blocking -- so this program only handles
    one connection at a time.
*/

// #include <entt.hpp>
// #include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include "utils.h"

constexpr auto WELCOME_MSG = "Hello client!";
constexpr auto MESSAGESIZE = 40;
auto serverPort = 53000;

int main() {
  /*
    // Initialise the window and draw a simple shape to test that it works.
    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFML Server");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Red);
    shape.setPosition(sf::Vector2f(300, 200)); // x = 300.0f, y = 200.0f
  */
  Utils::printMsg("Echo server startup...");

  // Create a TCP socket that we'll use to listen for incoming connections.
  sf::TcpListener listenerSocket;
  sf::Socket::Status listenStatus = listenerSocket.listen(serverPort);

  // Make the socket listen for connections.
  if (listenStatus == sf::Socket::Status::Done) {
    Utils::printMsg("Listening on port " + std::to_string(serverPort));
  } else {
    Utils::printMsg("Error binding listener socket!", error);
    return 1;
  }

  // Main loop will run while the window is open.
  // while (window.isOpen()) {
  while (true) {
    /*
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
    */
    // -----------------------------------------------------------------------------
    // Normally you want to draw frames after doing the input/logic/networking,
    // but we do it first so that we could check that the window works as
    // expected.
    // -----------------------------------------------------------------------------

    // Create new socket object for commnication with the client.
    sf::TcpSocket clientSocket;
    Utils::printMsg("Waiting for new client connection...");

    // Accept a new connection to the server socket.
    // This will update the clientSocket with new connection.
    sf::Socket::Status status = listenerSocket.accept(clientSocket);
    if (status == sf::Socket::Status::Done) {
      std::string message = "Connection accepted from ";
      sf::IpAddress incoming_ip = *clientSocket.getRemoteAddress();
      auto port = clientSocket.getRemotePort();

      Utils::printMsg(
          message + incoming_ip.toString() + ":" + std::to_string(port),
          success);
      // We don't need the socket after we accepted 1 client. We can only handle
      // 1 for now.
      // listenerSocket.close();
    } else {
      Utils::printMsg("Error accepting connection!", error);
      // window.close();
      // FIXME: in a real server, we wouldn't want the server to crash if
      // it failed to accept a connection -- recover more effectively!
      continue;  // Go back to waiting for a new connection.
    }

    // We'll use this array to hold the messages we exchange with the client.
    char buffer[MESSAGESIZE];

    // Fill the buffer with ~ characters to start with.
    memset(buffer, '~', MESSAGESIZE);
    // Replace part of the data with our welcome message.
    memcpy(buffer, WELCOME_MSG, strlen(WELCOME_MSG));

    // Send the welcome message to the client.
    Utils::printMsg("Sending welcome message...");
    Utils::printMsg(std::string(buffer).substr(0, MESSAGESIZE), debug);
    // clientSocket.send(buffer, MESSAGESIZE);

    // FIXME: check for errors from send
    sf::Socket::Status sendStatus = clientSocket.send(buffer, MESSAGESIZE);
    if (sendStatus != sf::Socket::Status::Done) {
      Utils::printMsg("Error sending welcome message!", error);
      clientSocket.disconnect();
      continue;  // Go back to waiting for a new connection.
    }

    while (true) {
      std::size_t message_size;
      // Receive as much data from the client as will fit in the buffer.
      // clientSocket.receive(buffer, sizeof(buffer), message_size);
      // FIXME: check for socket errors from recieve
      // check for closed connection
      sf::Socket::Status receiveStatus =
          clientSocket.receive(buffer, sizeof(buffer), message_size);
      if (receiveStatus != sf::Socket::Status::Done) {
        if (receiveStatus == sf::Socket::Status::Disconnected) {
          Utils::printMsg("Client disconnected.", warning);
        } else if (receiveStatus == sf::Socket::Status::Error) {
          Utils::printMsg("Error receiving message!", error);
        } else {
          Utils::printMsg("Unexpected socket status", error);
        }
        break;  // Go back to waiting for a new connection.
      }
      // check for strange-sized message
      if (message_size > MESSAGESIZE || message_size == 0) {
        Utils::printMsg(
            "Unexpected message size: " + std::to_string(message_size), error);
        break;  // Go back to waiting for a new connection.
      }
      // Print the message we recieved.
      Utils::printMsg("Message recieved: ", success);
      Utils::printMsg(std::string(buffer).substr(0, MESSAGESIZE), debug);

      // Send the same data back to the client.
      Utils::printMsg("Sending echo...");
      // clientSocket.send(buffer, MESSAGESIZE);
      // FIXME: check for errors from send
      sf::Socket::Status sendStatus = clientSocket.send(buffer, MESSAGESIZE);
      if (sendStatus != sf::Socket::Status::Done) {
        if (sendStatus == sf::Socket::Status::Disconnected) {
          Utils::printMsg("Client disconnected while sending echo.", warning);
        } else {
          Utils::printMsg("Failed to send echo message!", error);
        }
        break;  // Go back to waiting for a new connection.
      }
    }
    // Cleanup the socket if not needed.
    clientSocket.disconnect();
  }
}
