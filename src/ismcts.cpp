#include "ismcts.h"
#include "player_random.h"
#include "config.h"
#include "rng.h"
#include <algorithm>
#include <math.h>

static const double INF = 1e6;
static const double EXPLORATION = 3;

ISMCTSNode::ISMCTSNode():
    ISMCTSNode(1) {}

ISMCTSNode::ISMCTSNode(int numCurrDeterms):
    visits(numCurrDeterms, 0),
    avaliable(numCurrDeterms, 0),
    totalReward(numCurrDeterms, 0) {}

double ISMCTSNode::priority(int mult, int currDeterm)
{
    if (visits[currDeterm] == 0) return INF;
    return mult * totalReward[currDeterm] / visits[currDeterm] + EXPLORATION * sqrt(log(avaliable[currDeterm]) / visits[currDeterm]);
}

double ISMCTSNode::explore(GameState gameState, int currDeterm, int selfDeterm, int oppDeterm, int numSelfDeterms, int numOppDeterms)
{
    ++visits[currDeterm];

    if (gameState.isTerminal() || visits[currDeterm] == 1)
    {
        PlayerRandom playerRandom;
        gameState.setPlayers(&playerRandom, &playerRandom);
        while (!gameState.isTerminal())
        {
            gameState.applyPlayerAction();
        }

        int result = gameState.result();
        totalReward[currDeterm] += result;
        return result;
    }

    int currPlayerMult = gameState.currentPlayer();
    std::vector<int> actions = gameState.validActions();
    std::shuffle(actions.begin(), actions.end(), RNG);

    int numActions = actions.size();
    std::vector<int> actionCodes(numActions);

    double maxPriority = 0;
    int actionIdx = numActions;

    int nextDeterm = currPlayerMult > 0 ? selfDeterm : oppDeterm;
    int numNextDeterms = currPlayerMult > 0 ? numSelfDeterms : numOppDeterms;

    for (int i = 0; i < numActions; ++i)
    {
        actionCodes[i] = gameState.actionCode(actions[i]);
        auto it = children.find(actionCodes[i]);
        if (it == children.end())
        {
            auto res = children.insert({actionCodes[i], ISMCTSNode(numNextDeterms)});
            it = res.first;
        }
        ++it->second.avaliable[nextDeterm];

        double priority =  it->second.priority(currPlayerMult, nextDeterm);
        if (i == 0 || priority > maxPriority)
        {
            actionIdx = i;
            maxPriority = priority;
        }
    }

    gameState.applyAction(actions[actionIdx]);

    ISMCTSNode& child = children[actionCodes[actionIdx]];

    double reward = child.explore(gameState, nextDeterm, selfDeterm, oppDeterm, numSelfDeterms, numOppDeterms);
    totalReward[currDeterm] += reward;
    return reward;
}

std::vector<int> ISMCTSNode::scoreActions(GameState gameState, const std::vector<int>& actions, int currDeterm)
{
    int numActions = actions.size();
    std::vector<int> actionScores(numActions);
    for (int i = 0; i < numActions; ++i)
    {
        int actionCode = gameState.actionCode(actions[i]);
        actionScores[i] = children[actionCode].visits[currDeterm];
    }
    return actionScores;
}
