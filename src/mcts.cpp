#include "mcts.h"
#include "player_random.h"
#include "config.h"
#include "rng.h"
#include <algorithm>
#include <math.h>

static const double INF = 1e6;
static const double EXPLORATION = 3;

double MCTSNode::priority(int mult, bool parentSR)
{
    if (visits[parentSR] == 0) return INF + randInt(0, INF);
    return mult * totalReward[parentSR] / visits[parentSR] + EXPLORATION * sqrt(log(avaliable[parentSR]) / visits[parentSR]);
}

double MCTSNode::explore(GameState& gameState, bool selfRedetermed, bool parentSR, bool experimental)
{
    ++visits[parentSR];

    if (gameState.isTerminal() || visits[parentSR] == 1)
    {
        PlayerRandom playerRandom;
        gameState.setPlayers(&playerRandom, &playerRandom);
        int result = gameState.playToTerminal();;
        totalReward[parentSR] += result;
        return result;
    }

    int currPlayerMult = gameState.currentPlayer();
    std::vector<int> actions = gameState.validActions();

    int numActions = actions.size();
    std::vector<int> actionCodes(numActions);

    double maxPriority = -INF;
    int bestAction = M_NONE;
    MCTSNode* bestChild = nullptr;

    bool nextSR = currPlayerMult == 1 ? selfRedetermed : false;

    for (int action : actions)
    {
        int actionCode = gameState.actionCode(action);
        auto it = children.find(actionCode);
        if (it == children.end())
        {
            auto res = children.insert({actionCode, MCTSNode()});
            it = res.first;
        }
        ++it->second.avaliable[nextSR];

        double priority = it->second.priority(currPlayerMult, nextSR);
        if (priority > maxPriority)
        {
            maxPriority = priority;
            bestAction = action;
            bestChild = &(it->second);
        }
    }

    MCTSNode& child = *bestChild;
    gameState.applyAction(bestAction);

    double reward = child.explore(gameState, selfRedetermed, nextSR, experimental);
    totalReward[parentSR] += reward;
    return reward;
}

std::vector<int> MCTSNode::scoreActions(const GameState& gameState, const std::vector<int>& actions)
{
    int numActions = actions.size();
    std::vector<int> actionScores(numActions);
    for (int i = 0; i < numActions; ++i)
    {
        int actionCode = gameState.actionCode(actions[i]);
        actionScores[i] = children[actionCode].visits[false];
    }
    return actionScores;
}
