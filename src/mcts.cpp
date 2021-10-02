#include "mcts.h"
#include "config.h"
#include "rng.h"
#include <algorithm>
#include <math.h>

static const double INF = 1e6;
static const double EXPLORATION = 3;

static std::vector<MCTSNode> allNodes;

void MCTSNode::resetNodes(int numNodes)
{
    allNodes.clear();
    allNodes.reserve(numNodes);
    allNodes.push_back(MCTSNode());
}

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
        int result = gameState.randPlayToTerminal();
        totalReward[parentSR] += result;
        return result;
    }

    int currPlayerMult = gameState.currentPlayer();
    std::vector<int> actions = gameState.recommendedActions(experimental);

    double maxPriority = -INF;
    int bestAction = M_NONE;
    MCTSNode* bestChild = nullptr;

    bool nextSR = currPlayerMult == 1 ? selfRedetermed : false;

    for (int action : actions)
    {
        int actionCode = gameState.actionCode(action);

        auto res = children.insert({actionCode, &allNodes.back()});
        MCTSNode* child = res.first->second;
        if (res.second) allNodes.push_back(MCTSNode());

        ++child->avaliable[nextSR];
        double priority = child->priority(currPlayerMult, nextSR);
        if (priority > maxPriority)
        {
            maxPriority = priority;
            bestAction = action;
            bestChild = child;
        }
    }

    gameState.applyAction(bestAction);
    double reward = bestChild->explore(gameState, selfRedetermed, nextSR, experimental);
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
        actionScores[i] = children[actionCode]->visits[false];
    }
    return actionScores;
}
