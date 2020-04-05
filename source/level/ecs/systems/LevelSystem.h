
#ifndef GAME_LEVELSYSTEM_H
#define GAME_LEVELSYSTEM_H

class Level;

class LevelSystem
{
  public:
    const std::string name;

    LevelSystem(std::string name) : name(std::move(name)) {}

  protected:

    float updateFrequency = 0; // update this system every n seconds. if n = 0 then update(deltaTime) is called, else update(n)
    float updateAccumulator = 0;

    bool disabled = false;

    virtual void init(Level *lvl) {};

    virtual void update(double deltaTime, Level *lvl) = 0;

    friend Level;

    virtual ~LevelSystem() = default;

};


#endif
