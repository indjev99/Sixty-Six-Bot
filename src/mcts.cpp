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
    if (visits[parentSR] == 0) return INF;
    return mult * totalReward[parentSR] / visits[parentSR] + EXPLORATION * sqrt(log(avaliable[parentSR]) / visits[parentSR]);
}

double MCTSNode::explore(GameState gameState, bool selfRedetermed, bool parentSR)
{
    ++visits[parentSR];

    if (gameState.isTerminal() || visits[parentSR] == 1)
    {
        PlayerRandom playerRandom;
        gameState.setPlayers(&playerRandom, &playerRandom);
        while (!gameState.isTerminal())
        {
            gameState.applyPlayerAction();
        }

        int result = gameState.result();
        totalReward[parentSR] += result;
        return result;
    }

    int currPlayerMult = gameState.currentPlayer();
    std::vector<int> actions = gameState.validActions();
    std::shuffle(actions.begin(), actions.end(), RNG);

    int numActions = actions.size();
    std::vector<int> actionCodes(numActions);

    double maxPriority = -INF;
    int actionIdx = numActions;

    bool nextSR = currPlayerMult > 0 ? selfRedetermed : false;

    for (int i = 0; i < numActions; ++i)
    {
        actionCodes[i] = gameState.actionCode(actions[i]);
        auto it = children.find(actionCodes[i]);
        if (it == children.end())
        {
            auto res = children.insert({actionCodes[i], MCTSNode()});
            it = res.first;
        }
        ++it->second.avaliable[nextSR];

        double priority =  it->second.priority(currPlayerMult, nextSR);
        if (i == 0 || priority > maxPriority)
        {
            actionIdx = i;
            maxPriority = priority;
        }
    }

    gameState.applyAction(actions[actionIdx]);

    MCTSNode& child = children[actionCodes[actionIdx]];

    double reward = child.explore(gameState, selfRedetermed, nextSR);
    totalReward[parentSR] += reward;
    return reward;
}

std::vector<int> MCTSNode::scoreActions(GameState gameState, const std::vector<int>& actions)
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
