#pragma once

#include "game_state.h"
#include <map>

#define NUM_REDETERM_OPTIONS 2

struct MCTSNode
{
    double explore(GameState gameState, bool selfRedetermed, bool parentSR);
    std::vector<int> scoreActions(GameState gameState, const std::vector<int>& actions);

    void debug(GameState gameState, bool selfRedetermed, bool parentSR);

private:

    double priority(int mult, bool parentSR);

    int visits[NUM_REDETERM_OPTIONS];
    int avaliable[NUM_REDETERM_OPTIONS];
    double totalReward[NUM_REDETERM_OPTIONS];
    std::map<int, MCTSNode> children;
};
