#pragma once

#include "game_state.h"

#define NUM_REDETERM_OPTIONS 2

struct MCTSNode
{
    static void resetNodes(int numPlayouts);

    MCTSNode();

    double explore(GameState& gameState, bool selfRedetermed, bool parentSR, bool experimental);
    std::vector<int> scoreActions(const GameState& gameState, const std::vector<int>& actions);

    void debug(GameState& gameState, bool selfRedetermed);

private:

    double priority(int mult, bool parentSR, double heuristic);

    int visits[NUM_REDETERM_OPTIONS];
    int avaliable[NUM_REDETERM_OPTIONS];
    double totalReward[NUM_REDETERM_OPTIONS];
    MCTSNode* children[NUM_ACODES];
};
