
#ifndef GAME_PHYSICSSYSTEM_H
#define GAME_PHYSICSSYSTEM_H


#include "LevelSystem.h"
#include "../../Level.h"
#include "../components/Physics.h"

namespace
{
    enum Move
    {
        up, down, left, right, none
    };
}

class PhysicsSystem : public LevelSystem
{
  protected:
    void init(Level *lvl) override
    {
        updateFrequency = 1. / 60.;
    }

    void update(double deltaTime, Level *lvl) override
    {
        lvl->entities.view<Physics>().each([&](Physics &physics) {

            physics.velocity.y -= physics.gravity * deltaTime;

            vec2 pixelsToMove = vec2(physics.velocity) * vec2(deltaTime);
            pixelsToMove += physics.velocityAccumulator; // add remains of previous update.

            while (true)
            {
                Move toDo;
                if (pixelsToMove.x >= 1)         toDo = Move::right;
                else if (pixelsToMove.x <= -1)   toDo = Move::left;
                else if (pixelsToMove.y >= 1)    toDo = Move::up;
                else if (pixelsToMove.y <= -1)   toDo = Move::down;
                else break;

                ivec2 prevPos = physics.body.center;

                if (tryMove(physics, toDo)) // move succeeded -> decrease pixelsToMove:
                    pixelsToMove -= physics.body.center - prevPos;
                else // move is not possible:
                {
                    if (toDo == Move::left || toDo == Move::right)  pixelsToMove.x = 0; // body cannot move horizontally anymore.
                    else                                            pixelsToMove.y = 0; // body cannot move vertically anymore.
                }
            }
            physics.velocityAccumulator = pixelsToMove; // store remains for next update
        });
    }

  private:

    bool tryMove(Physics &physics, Move toDo)
    {
        std::vector<Move> movesToDo;

        while (toDo != Move::none)
        {
            movesToDo.push_back(toDo);

            if (!canDoMove(physics, toDo)) return false;
        }
        for (auto move : movesToDo)
            doMove(physics, move);
        return true;
    }

    bool canDoMove(Physics &p, Move &moveToDo)
    {
        moveToDo = Move::none;
        return true;
    }

    void doMove(Physics &p, Move move)
    {
        switch (move)
        {
            case up:    p.body.center.y++; break;
            case down:  p.body.center.y--; break;
            case left:  p.body.center.x--; break;
            case right: p.body.center.x++; break;
            case none:                     break;
        }
    }

};


#endif
