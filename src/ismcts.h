#pragma once

#include "game_state.h"
#include <map>

struct ISMCTSNode
{
    ISMCTSNode();
    ISMCTSNode(int numCurrDeterms);

    double explore(GameState gameState, int currDeterm, int selfDeterm, int oppDeterm, int numSelfDeterms, int numOppDeterms);
    std::vector<int> scoreActions(GameState gameState, const std::vector<int>& actions, int currDeterm);

    void debug(GameState gameState, int currDeterm, int selfDeterm, int oppDeterm);

private:

    double priority(int mult, int currDeterm);

    std::vector<int> visits;
    std::vector<int> avaliable;
    std::vector<double> totalReward;
    std::map<int, ISMCTSNode> children;
};
