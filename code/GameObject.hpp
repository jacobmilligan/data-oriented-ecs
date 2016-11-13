//
// GameObject.hpp
// ECS_Framework
//
// ----------------------------------------------------------------------------
//
// Created by Jacob Milligan on 2/11/2016.
// Copyright (c) 2016 Jacob Milligan All rights reserved.
//

#ifndef ECS_FRAMEWORK_GAMEOBJECT_HPP
#define ECS_FRAMEWORK_GAMEOBJECT_HPP

#include <SFML/Graphics.hpp>

namespace ecs {

/// \brief GameObject is the base class for all game objects to inherit from
class GameObject {
public:
  /// Initializes a new GameObject with the given transform properties
  /// \param position Position of this game object in the world
  /// \param scale Scale of this game objects transform
  GameObject(sf::Vector2f position, sf::Vector2f scale)
    : position_(position), scale_(scale) {}

  /// \brief Gets the position of the GameObject in the world
  /// \return The game objects position vector
  inline sf::Vector2f position()
  {
    return position_;
  }

  /// \brief Gets the GameObject instances scale transform
  /// \return The game objects scale vector
  inline sf::Vector2f scale()
  {
    return scale_;
  }

  /// \brief Updates this GameObject. Must be implemented by derived classes
  virtual void update() = 0;

  /// \brief Draws this GameObject. Must be implemented by derived classes.
  virtual void draw(sf::RenderWindow &window) = 0;

  /// \brief Sets the position in the game world of this GameObject
  /// \param x World x coordinate
  /// \param y World y coordinate
  virtual inline void set_position(float x, float y)
  {
    position_.x = x;
    position_.y = y;
  }

  /// \brief Sets the scale transform of this GameObject
  /// \param x x scale
  /// \param y y scale
  virtual inline void set_scale(float x, float y)
  {
    scale_.x = x;
    scale_.y = y;
  }

private:

  /// \brief The GameObjects world position
  sf::Vector2f position_;
  /// \brief The GameObjects scale transform vector
  sf::Vector2f scale_;
};

/// \brief Drawable is a GameObject with a sprite that can be rendered to a window
class Drawable : public GameObject {
public:

  /// \brief Initializes a new Drawable with the specified texture and transforms
  /// \param texture Texture to assign to this objects sprite
  /// \param position Position of this object in the world
  /// \param scale Scale of this objects transform
  Drawable(sf::Texture &texture, sf::Vector2f position, sf::Vector2f scale)
    : GameObject(position, scale)
  {
    sprite_.setTexture(texture);
    sprite_.setPosition(position);
  }

  /// \brief Moves this drawable one px to the right and down
  void update() override
  {
    sprite_.move(1, 1);
  }

  /// \brief Draws this objects sprite to a window
  /// \param window Window to draw the sprite to
  void draw(sf::RenderWindow &window) override
  {
    window.draw(sprite_);
  }

  /// \brief Sets the position in the game world of this GameObject
  /// \param x World x coordinate
  /// \param y World y coordinate
  inline void set_position(float x, float y) override
  {
    sprite_.setPosition(x, y);
  }

  /// \brief Sets the scale transform of this GameObject
  /// \param x x scale
  /// \param y y scale
  inline void set_scale(float x, float y) override
  {
    sprite_.setScale(x, y);
  }

protected:
  /// \brief The sprite to apply transforms to and render to the window
  sf::Sprite sprite_;
};

/// \brief Character is a Drawable GameObject with an AABB box
/// that can be used for collision checking
class Character : public Drawable {
public:

  /// \brief Initializes a new Character with the specified texture and transforms
  /// \param texture Texture to assign to this objects sprite
  /// \param position Position of this object in the world
  /// \param scale Scale of this objects transform
  Character(sf::Texture &texture, sf::Vector2f position, sf::Vector2f scale, sf::FloatRect rect)
  : intersect_(false), check_(rect), Drawable(texture, position, scale)
  {
    aabb_ = sprite_.getLocalBounds();
  }

  /// \brief Currently only resets the Character's AABB to a new sf::FloatRect
  void update_aabb()
  {
    intersect_ = aabb_.intersects(check_);
  }

  /// \brief Currently does nothing but would check collision in a proper game
  void check_collision(/*CollisionGrid* grid*/)
  {
    // ...
  }

private:
  /// \brief This Characters AABB collision box
  sf::FloatRect aabb_, check_;
  bool intersect_;
};

}

#endif //ECS_FRAMEWORK_GAMEOBJECT_HPP
