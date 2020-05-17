
#ifndef GAME_ENTITYSYSTEM_H
#define GAME_ENTITYSYSTEM_H

#include <string>
#include <iostream>

class Room;

/**
 * Base class for all entity systems.
 *
 * An entity system should be used for updating components of entities.
 */
class EntitySystem
{
  public:
    const std::string name;

    EntitySystem(std::string name) : name(std::move(name)) {}

  protected:

    int updateFrequency = 0; // update this system n times per second. if n = 0 then update(deltaTime) is called, else update(1/n)
    float updateAccumulator = 0;

    bool disabled = false;

    virtual void init(Room *room) {};

    virtual void update(double deltaTime, Room *room) = 0;

    friend Room;

    virtual ~EntitySystem() = default;

};


#endif
