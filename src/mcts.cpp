#include "mcts.h"
#include "config.h"
#include "rng.h"
#include <algorithm>
#include <math.h>

static const double INF = 1e6;
static const double EXPLORATION = 3;
static const double HEUR_WEIGHT = 20;

// TODO: Play around with EXPLORATION

static std::vector<MCTSNode> allNodes;

void MCTSNode::resetNodes(int numPlayouts)
{
    allNodes.clear();
    allNodes.reserve(numPlayouts * (HAND_SIZE + NUM_SPEC_MOVES));
}

MCTSNode::MCTSNode()
{
    std::fill(visits, visits + NUM_REDETERM_OPTIONS, 0);
    std::fill(avaliable, avaliable + NUM_REDETERM_OPTIONS, 0);
    std::fill(totalReward, totalReward + NUM_REDETERM_OPTIONS, 0);
    std::fill(children, children + NUM_ACODES, nullptr);
}

double MCTSNode::priority(int mult, bool parentSR, double heuristic)
{
    if (visits[parentSR] == 0) return INF + heuristic + randInt(0, INF) * 1.0 / INF / INF;
    return (mult * totalReward[parentSR] + HEUR_WEIGHT * heuristic) / visits[parentSR] + EXPLORATION * sqrt(log(avaliable[parentSR]) / visits[parentSR]);
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

        if (children[actionCode] == nullptr)
        {
            allNodes.push_back(MCTSNode());
            children[actionCode] = &allNodes.back();
        }
        MCTSNode* child = children[actionCode];

        ++child->avaliable[nextSR];
        double priority = child->priority(currPlayerMult, nextSR, gameState.actionHeuristic(action));
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
