#pragma once

#include "game_state.h"
#include <map>

struct MCTSNode
{
    MCTSNode();

    double explore(GameState gameState);
    std::vector<int> scoreActions(GameState gameState, const std::vector<int>& actions);

    void debug(GameState gameState);

private:

    double priority(int mult);

    int visits;
    int avaliable;
    double totalReward;
    std::map<int, MCTSNode> children;
};
