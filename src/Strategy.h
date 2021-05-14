#pragma once

#include <queue>
#include <sc2api/sc2_unit.h>

// TODO: do we need a struct which has the ability id, but other information such as if the step should be blocking?
//       might be a nice idea if we also wanted to include tech/mineral/gas requirements
class Strategy{
    public:
    Strategy() {};
    
    // add initial steps to build orders
    virtual void initialize(); // FIXME: does this need {} here? 

    // add more steps if needed
    void pushPriorityStep(sc2::ABILITY_ID ability);
    void pushOptionalStep(sc2::ABILITY_ID ability);

    // get the next step in order and pop it
    sc2::ABILITY_ID getNextPriorityStep();
    sc2::ABILITY_ID getNextOptionalStep();

    // look at the next step but don't remove it yet
    sc2::ABILITY_ID peekNextPriorityStep();
    sc2::ABILITY_ID peekNextOptionalStep();

    protected:
    std::queue<sc2::ABILITY_ID> priorityBuildOrder;
    std::queue<sc2::ABILITY_ID> optionalBuildOrder;
    int maxWorkers; // most useful for 1 base all ins. if this is negative it should be ignored
};