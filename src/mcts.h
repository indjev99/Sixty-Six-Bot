#pragma once

#include "game_state.h"
#include <map>

struct MCTSNode
{
    MCTSNode();

    double explore(GameState gameState, int selfPoints, int oppPoints);
    int choseAction(GameState gameState);

    void debug(GameState gameState);

private:

    double priority(int mult);

    int visits;
    int avaliable;
    double totalReward;
    std::map<int, MCTSNode> children;
};
