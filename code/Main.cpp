//
// Main.cpp
// ECS_Framework
//
// ----------------------------------------------------------------------------
//
// Created by Jacob Milligan on 29/10/2016.
// Copyright (c) 2016 Jacob Milligan All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>

#include <SFML/Graphics.hpp>
#include <FPSCounter.hpp>
#include <GameObject.hpp>
#include <Entity.hpp>

/// \brief Fills a vector of sprites with the needed data
/// \param sprites Vector to fill
/// \param numEntities Number of entities to create
/// \param texture Texture to assign to each sprite
void setup_raw(std::vector<sf::Sprite> &sprites, const int numEntities, sf::Texture &texture)
{
  auto textureSize = texture.getSize();

  for ( int i = 0; i < numEntities; ++i ) {
    sprites.emplace_back(texture);
    sprites.back().setPosition(i * textureSize.x, i);
  }
}

/// \brief Sets up all game objects with the necessary data
/// \param gameObjects Container to setup
/// \param numEntities Number of entities to create
/// \param texture Texture to assign to each sprite
void setup_oo(std::vector<ecs::Character> &gameObjects, const int numEntities, sf::Texture &texture, sf::FloatRect &rect)
{
  auto textureSize = texture.getSize();

  for ( int i = 0; i < numEntities; ++i ) {
    gameObjects.emplace_back(
      texture, sf::Vector2f(i * textureSize.x, i), sf::Vector2f(1, 1), rect
    );
  }
}

/// \brief Updates the game objects calling their update, draw etc. functions in the
/// correct order
/// \param gameObjects Container to update
/// \param window Window to draw the game objects sprites to
void update_game_objects(std::vector<ecs::Character> &gameObjects, sf::RenderWindow &window)
{
  for ( int i = 0; i < gameObjects.size(); ++i ) {
    gameObjects[i].update();
  }
  for ( int i = 0; i < gameObjects.size(); ++i ) {
    gameObjects[i].update_aabb();
  }
  for ( int i = 0; i < gameObjects.size(); ++i ) {
    gameObjects[i].draw(window);
  }
}

/// Updates a sprite vector and draws them to the window
/// \param sprites Vector of sprites to update
/// \param window Window to draw sprites to
void update_sprites(std::vector<sf::Sprite> &sprites, sf::RenderWindow &window)
{
  auto size = sprites.size();
  for ( int i = 0; i < size; ++i ) {
    sprites[i].move(1, 1);
  }
  for ( int i = 0; i < size; ++i ) {
    window.draw(sprites[i]);
  }
}

/// \brief Sets up the ECS with the needed data
/// \param ecs EntityMap to setup
/// \param numEntities Number of entities to create
/// \param texture Texture to allocate to each entities Sprite component
void setup_dod(ecs::EntityMap &ecs, const int numEntities, sf::Texture &texture)
{
  auto render = ecs.add_system<ecs::SpriteSystem>();
  auto movement = ecs.add_system<ecs::CollisionSystem>();
  auto textureSize = texture.getSize();

  for ( int i = 0; i < numEntities; ++i ) {
    auto e = ecs.create();
    ecs.attach<ecs::SpriteSystem>(e);
    ecs.attach<ecs::CollisionSystem>(e);

    render->spriteData.get(e)->setTexture(texture);
    render->spriteData.get(e)->setPosition(i * textureSize.x, i);

    *(movement->boxes.get(e)) = render->spriteData.get(e)->getLocalBounds();
  }
}

/// \brief Entry point
int main(int argc, char** argv) {

  auto numEntities = UINT16_MAX;
  
  auto testType = 0;
  auto iteration = 0;
  std::vector<std::tuple<int, float>> averages;

  // Get the desktops current resolution and divide it by 1.3
  // to make it smaller
  auto mode = sf::VideoMode::getDesktopMode();

  // Open a new window with the video mode and the title 'Entity Framework'
  sf::RenderWindow window(mode, "Entity Framework");

  sf::Texture texture;
  texture.loadFromFile(
    "/Users/jacobmilligan/Uni/OOP/ResearchReport/code/images/playership_blue.png"
  );

  std::vector<sf::Sprite> rawSprites;
  ecs::EntityMap ecs;
  std::vector<ecs::Character> testObjects;

  sf::FloatRect check(10, 10, 10, 10);

  setup_dod(ecs, numEntities, texture);
  setup_oo(testObjects, numEntities, texture, check);
  setup_raw(rawSprites, numEntities, texture);

  ecs::FPSCounter fps;
  ecs::FPSCounter sampleFps;

  std::ofstream csv(
    "/Users/jacobmilligan/Uni/OOP/ResearchReport/code/data.csv"
  );
  csv << "Experiment,Sample,Average Frame Time\n";

  while ( window.isOpen() && testType < 3 ) {

    while ( sampleFps.total_frames() <= 60.0f ) {
      sf::Event event;

      while ( window.pollEvent(event) ) {

        if ( event.type == sf::Event::Closed )
          window.close();

      }

      window.clear(sf::Color::Black);

      switch (testType) {
        case 0:
          update_sprites(rawSprites, window);
          break;
        case 1:
          update_game_objects(testObjects, window);
          break;
        case 2:
          ecs.get_system<ecs::SpriteSystem>()->move();
          ecs.get_system<ecs::CollisionSystem>()->update_collision(check);
          ecs.get_system<ecs::SpriteSystem>()->render(window);
          break;
      }

      fps.update();
      sampleFps.update();

      window.display();
    }

    csv << testType << "," << iteration
        << "," << sampleFps.average_delta_time() << "\n";

    iteration++;
    sampleFps.reset();

    if ( iteration > 19 ) {
      averages.emplace_back(testType, fps.average_delta_time());
      iteration = 0;
      testType++;
      fps.reset();
    }
  }

  window.close();

  csv.close();

  return 0;
}