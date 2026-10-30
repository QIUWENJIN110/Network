#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

#include "tank_message.h"

class Tank {
 public:
  // Colour string will be used in path for image texture loading.
  // Will work with "red", "blue", "green" and "black".
  //
  // FIXME: this is not the cleanest solution as you can make a typo which will
  // cause texture to fail to load. Ideally should use enum/map or similar
  // solution.
  Tank(std::string colour);

  void Update(float dt);
  void Render(sf::RenderWindow& window);

  sf::Vector2f position = {0.f, 0.f};
  sf::Angle barrelRotation = sf::degrees(0);
  sf::Angle bodyRotation = sf::degrees(0);

  struct {
    bool forward = false;
    bool backward = false;
    bool left = false;
    bool right = false;
  } isMoving;

 private:
  // Temporary placeholder texture, make sue to replace before rendering the
  // sprite.
  sf::Texture placeholder = sf::Texture(sf::Vector2u(1, 1));

  sf::Texture bodyTexture;
  sf::Texture barrelTexture;

  // These can (and probably should) be replaced with std::optional or unique
  // pointers, to remove the need to use placeholder textures for sprite
  // initialisation.
  sf::Sprite body = sf::Sprite(placeholder);
  sf::Sprite barrel = sf::Sprite(placeholder);

  float movementSpeed = 150.f;
  float rotationSpeed = 200.f;

  // Saving current colour here in case we need to send elsewhere.
  std::string colorString = "";
};
