
#ifndef GAME_PHYSICSSYSTEM_H
#define GAME_PHYSICSSYSTEM_H


#include "../LevelSystem.h"
#include "../../../Level.h"
#include "../../components/Physics.h"

namespace
{
    enum Move
    {
        up, down, left, right, none
    };
}

class PhysicsSystem : public LevelSystem
{
    using LevelSystem::LevelSystem;
  protected:
    void update(double deltaTime, Level *lvl) override
    {
        collisionDetector = new TerrainCollisionDetector(lvl->getCurrentRoom());

        lvl->entities.view<Physics>().each([&](Physics &physics) {

            updatePosition(physics, deltaTime);
            updateVelocity(physics, deltaTime);
        });
    }

  private:
    TerrainCollisionDetector *collisionDetector;

    /**
     * Updates the velocity of a body.
     */
    void updateVelocity(Physics &physics, double deltaTime)
    {
        physics.velocity.y -= physics.gravity * deltaTime;

        if (physics.touches.floor && physics.velocity.y < 0) physics.velocity.y = 0;
        if (physics.touches.ceiling && physics.velocity.y > 0) physics.velocity.y = 0;

        if (physics.touches.leftWall && physics.velocity.x < 0) physics.velocity.x = 0;
        if (physics.touches.rightWall && physics.velocity.x > 0) physics.velocity.x = 0;
    }

    /**
     * Updates the position of a body using its velocity
     */
    void updatePosition(Physics &physics, double deltaTime)
    {
        vec2 pixelsToMove = vec2(physics.velocity) * vec2(deltaTime);
        pixelsToMove += physics.velocityAccumulator; // add remains of previous update.
        bool moved = false;
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
            {
                moved = true;
                pixelsToMove -= physics.body.center - prevPos;
            } // move is not possible:
            else if (toDo == Move::left || toDo == Move::right)  pixelsToMove.x = 0; // cant move horizontally anymore.
            else                                                 pixelsToMove.y = 0; // cant move vertically anymore.
        }
        physics.velocityAccumulator = pixelsToMove; // store remains for next update
        if (!moved) updateTerrainCollisions(physics); // terrain might have changed
    }

    /**
     * Will try to do the move, returns true if success
     */
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

    /**
     * Checks if a move is possible.
     * Sometimes a move is possible but requires another move to be done as well,
     * therefore `moveToDo` will be changed to that other move (or Move::none)
     */
    bool canDoMove(Physics &p, Move &moveToDo)
    {
        Move originalMove = moveToDo;
        moveToDo = Move::none;

        switch (originalMove)
        {
            case up:    return !p.touches.ceiling;
            case down:  return !p.touches.floor;
            case left:
                if (p.touches.slopeDown)
                    moveToDo = Move::up;
                return !p.touches.leftWall;

            case right:
                if (p.touches.slopeUp)
                    moveToDo = Move::up;
                return !p.touches.rightWall;

            case none:  return true;
        }
        return true;
    }

    /**
     * Executes the move (without checking if the move is possible to do)
     */
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
        updateTerrainCollisions(p);
    }

    void updateTerrainCollisions(Physics &p)
    {
        AABB outlineBox = p.body;
        outlineBox.halfSize += 1; // make box 1 pixel larger to detect if p.body *touches* terrain
        p.touches = collisionDetector->detect(outlineBox, p.ignorePlatforms);
    }

};

#endif
