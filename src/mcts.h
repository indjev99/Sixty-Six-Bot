#pragma once

#include "game_state.h"
#include <map>

struct MCTSNode
{
    MCTSNode();

    double explore(GameState gameState);
    int choseAction(GameState gameState);

private:

    double priority(int mult);

    int visits;
    int avaliable;
    double totalReward;
    std::map<int, MCTSNode> children;
};
