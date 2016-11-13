//
// Entity.hpp
// ECS_Framework
//
// ----------------------------------------------------------------------------
//
// Created by Jacob Milligan on 4/11/2016.
// Copyright (c) 2016 Jacob Milligan All rights reserved.
//

#ifndef ECS_FRAMEWORK_COMPONENTDATA_HPP
#define ECS_FRAMEWORK_COMPONENTDATA_HPP

#include <cstdint>
#include <vector>
#include <typeindex>
#include <unordered_map>

#include <SFML/Graphics.hpp>

namespace ecs {

/// \brief Entity is an id and index pair that map this instance
/// to a series of systems and components, tying them all together
struct Entity {
  uint16_t id;
  uint16_t index;
  uint16_t generation;
};

/// \brief ComponentData is a container of components with garunteed contiguous
/// storage that can be queried with an Entity id
/// \tparam Type of component to store
template <typename T>
struct ComponentData {

  /// \brief All instances of this component mapped to an entity
  std::vector<T> instances;

  /// \brief Initializes the container and lookup table
  ComponentData()
  {
    for ( int e = 0; e < UINT16_MAX; ++e ) {
      lookup_[e].id = e;
      lookup_[e].index = UINT16_MAX;
      lookup_[e].generation = UINT16_MAX;
    }
  }

  /// \brief Gets an entities associated component as a pointer.
  /// \details Uses a pointer rather than a reference to allow for better
  /// auto compatibility and checking against nullptr
  /// \param entity The entity whose component is being retrieved
  /// \return Pointer to the component instance if found, nullptr otherwise
  inline T* get(const Entity& entity)
  {
    auto lookup = lookup_[entity.id];
    if ( lookup.generation != entity.generation )
      return nullptr;

    return (lookup.index < UINT16_MAX) ? &instances[lookup.index] : nullptr;
  }

  /// \brief Checks to see if the entity has an instance of the component
  /// \param entity To check for
  /// \return True if found, false otherwise
  inline bool has_component(const Entity& entity)
  {
    auto lookup = lookup_[entity.id];
    if ( lookup.generation != entity.generation )
      return false;

    return lookup.index < instances.size() && lookup.index < UINT16_MAX;
  }

  /// \brief Attaches an instance of the component to an entity
  /// \param entity Entity to attach
  /// \return The entity with id and index into the data
  inline Entity attach(const Entity& entity)
  {
    instances.emplace_back();
    Entity e;
    e.index = instances.size() - 1;
    e.id = entity.id;
    e.generation = entity.generation;
    lookup_[e.id] = e;
    return e;
  }

  /// \brief Removes an entity from this container
  /// \param entity Entity to remove
  inline void detach(const Entity& entity)
  {
    auto index = lookup_[entity.id].index;
    std::swap(*get(entity), instances.back());
    instances.pop_back();
    lookup_[entity.id].index = UINT16_MAX;
  }

private:
  /// \brief Lookup array used to index into the actual data
  Entity lookup_[UINT16_MAX];
};

/// \brief System is a base class for application systems operating
/// some logic on a specific set of component data and handles adding/removing
/// entities from its ComponentData array.
struct System {

  /// \brief Adds a new entity to this systems component data. Must be
  /// implemented by all derived classes
  /// \param entity Entity to add
  virtual void add(const Entity& entity) = 0;

  /// \brief Removes an entity from this systems component data. Must be
  /// implemented by all derived classes
  /// \param entity Entity to remove
  virtual void remove(const Entity& entity) = 0;

  /// \brief Checks if this system contains a specified entity. Must be
  /// implemented by all derived classes
  /// \param entity Entity to check for
  /// \return True if has entity, false otherwise
  virtual bool has_entity(const Entity& entity) = 0;
};

/// \brief SpriteSystem operates on a set of sf::Sprite data, rendering them
/// to an sf::RenderWindow
struct SpriteSystem : public System {

  /// Sprite data to operate on
  ComponentData<sf::Sprite> spriteData;

  /// \brief Adds a new entity to this systems component data.
  /// \param entity Entity to add
  inline void add(const Entity& entity) override
  {
    spriteData.attach(entity);
  }

  /// \brief Removes an entity from this systems component data.
  /// \param entity Entity to remove
  inline void remove(const Entity& entity) override
  {
    spriteData.detach(entity);
  }

  /// \brief Checks if this system contains a specified entity
  /// \param entity Entity to check for
  /// \return True if has entity, false otherwise
  inline bool has_entity(const Entity& entity) override
  {
    return spriteData.has_component(entity);
  }

  /// \brief Moves all sprites 1px down and to the right
  void move()
  {
    auto size = spriteData.instances.size();
    for ( int e = 0; e < size; ++e ) {
      spriteData.instances[e].move(1, 1);
    }
  }

  /// Renders all sprites to a window
  /// \param window Widow to render to
  void render(sf::RenderWindow &window)
  {
    auto size = spriteData.instances.size();
    for ( int e = 0; e < size; ++e ) {
      window.draw(spriteData.instances[e]);
    }
  }

};

/// \brief CollisionSystem is a placeholder for a collision checking system.
// Currently it only reassigns a rectangle component
struct CollisionSystem : public System {

  /// The collision boxes to operate on
  ComponentData<sf::FloatRect> boxes;

  /// \brief Adds a new entity to this systems component data.
  /// \param entity Entity to add
  inline void add(const Entity& entity) override
  {
    boxes.attach(entity);
  }

  /// \brief Removes an entity from this systems component data.
  /// \param entity Entity to remove
  inline void remove(const Entity& entity) override
  {
    boxes.detach(entity);
  }

  /// \brief Checks if this system contains a specified entity
  /// \param entity Entity to check for
  /// \return True if has entity, false otherwise
  inline bool has_entity(const Entity& entity) override
  {
    return boxes.has_component(entity);
  }

  /// \brief Reassigns each collision box to a new sf::FloatRect
  void update_collision(const sf::FloatRect &rect)
  {
    auto size = boxes.instances.size();
    bool intersect = false;

    for ( int e = 0; e < size; ++e ) {
      intersect = boxes.instances[e].intersects(rect);
    }
  }

};

/// \brief EntityMap maps ComponentData, System, and Entity instances to one
/// another and coordinates creation, destruction, and component adding for all
/// entities. All operations on data should be done via this interface.
class EntityMap {
public:

  /// \brief Initializes a new EntityMap
  EntityMap() : nextId_(0), currentGeneration_(0), size_(0) {}

  /// \brief Registers a system to this EntityMap for convenient lookup
  /// \tparam Type of System to add
  /// \return Pointer to the System instance
  template <typename T>
  inline T* add_system()
  {
    systems_.emplace(typeid(T), std::make_unique<T>());
    return get_system<T>();
  }

  /// \brief Creates a new Entity
  /// \return The new entity
  inline Entity create()
  {
    size_++;
    return get_entity();
  }

  /// \brief Creates a new Entity with the specified tag
  /// \return The new entity
  inline Entity create(const std::string& tag)
  {
    Entity e = create();
    tags_[tag] = e;
    return e;
  }

  /// \brief Destroys an entity by removing it from all registered System instances
  /// \param entity Entity to remove
  void destroy(const Entity &entity)
  {
    size_--;
    for ( auto &s : systems_ ) {
      s.second->remove(entity);
    }
  }

  /// \brief Gets a tagged entity
  /// \param tag Tag to search for
  /// \return The entity
  Entity get_tagged_entity(const std::string& tag)
  {
    return tags_[tag];
  }

  /// \brief Attaches an Entity to the specified System instance
  /// \tparam Type of System to attach to
  /// \param entity Entity to attach
  template <typename T>
  inline void attach(const Entity &entity)
  {
    systems_[typeid(T)]->add(entity);
  }

  /// \brief Removes an Entity from the specified System instance
  /// \tparam Type of System to remove from
  /// \param entity Entity to remove
  template <typename T>
  inline void remove(const Entity& entity)
  {
    systems_[typeid(T)]->remove(entity);
  }

  /// \brief Gets a pointer to the specified registered System instance
  /// \tparam Type of System to get
  /// \return Pointer to the System
  template <typename T>
  inline T* get_system()
  {
    return static_cast<T*>(systems_[typeid(T)].get());
  }

  /// \brief Checks if an entity belongs to a specific system
  /// \param entity Entity to check
  /// \return True if belongs to system, false otherwise
  template<typename T>
  inline bool belongs_to(const Entity& entity)
  {
    return systems_[typeid(T)]->has_entity(entity);
  }

  /// \brief Gets the number of Entity instances currently alive
  /// \return Number of alive Entity instances
  inline uint64_t size() { return size_; }

private:
  /// \brief Map for lookup and retrieval of stored System instances
  std::unordered_map<std::type_index, std::unique_ptr<System>> systems_;
  /// \brief All tagged entities
  std::unordered_map<std::string, Entity> tags_;
  /// \brief The next available unique ID for assigning to Entity instances
  uint16_t nextId_;
  /// \brief The current generation of entity id's
  uint16_t currentGeneration_;
  /// \brief The number of currently living entities
  uint64_t size_;

  /// \brief Gets the next available Entity with id and generation
  /// \return
  inline Entity get_entity() {
    nextId_++;

    // Check for next generation
    if ( nextId_ == 0 )
      currentGeneration_++;

    Entity e;
    e.id = nextId_;
    e.index = UINT16_MAX;
    e.generation = currentGeneration_;

    return e;
  }
};

}

#endif //ECS_FRAMEWORK_COMPONENTDATA_HPP
