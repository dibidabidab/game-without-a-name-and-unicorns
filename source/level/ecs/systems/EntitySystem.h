
#ifndef GAME_ENTITYSYSTEM_H
#define GAME_ENTITYSYSTEM_H

#include <string>
#include <iostream>

class Room;

class EntitySystem
{
  public:
    const std::string name;

    EntitySystem(std::string name) : name(std::move(name)) {}

  protected:

    float updateFrequency = 0; // update this system every n seconds. if n = 0 then update(deltaTime) is called, else update(n)
    float updateAccumulator = 0;

    bool disabled = false;

    virtual void init(Room *room) {};

    virtual void update(double deltaTime, Room *room) = 0;

    friend Room;

    virtual ~EntitySystem() = default;

};


#endif
