#include "mcts.h"
#include "player_random.h"
#include "rng.h"
#include <algorithm>
#include <math.h>

static const double INF = 1e6;
static const double EXPLORATION = 4;

MCTSNode::MCTSNode():
    visits(0),
    avaliable(0),
    totalReward(0) {}

double MCTSNode::priority(int mult)
{
    if (visits == 0) return INF;
    return mult * totalReward / visits + EXPLORATION * sqrt(log(avaliable) / visits);
}

double MCTSNode::explore(GameState gameState)
{
    ++visits;

    if (gameState.isTerminal() || visits == 1)
    {
        PlayerRandom playerRandom;
        gameState.setPlayers(&playerRandom, &playerRandom);
        while (!gameState.isTerminal())
        {
            gameState.applyPlayerAction();
        }

        int result = gameState.result();
        totalReward += result;
        return result;
    }

    int currPlayerMult = gameState.currentPlayer();
    std::vector<int> actions = gameState.validActions();
    std::shuffle(actions.begin(), actions.end(), RNG);

    int numActions = actions.size();
    std::vector<int> actionCodes(numActions);

    double maxPriority = 0;
    int actionIdx = numActions;

    for (int i = 0; i < numActions; ++i)
    {
        actionCodes[i] = gameState.actionCode(actions[i]);
        auto it = children.find(actionCodes[i]);
        if (it == children.end())
        {
            auto res = children.insert({actionCodes[i], MCTSNode()});
            it = res.first;
        }
        ++it->second.avaliable;

        double priority =  it->second.priority(currPlayerMult);
        if (i == 0 || priority > maxPriority)
        {
            actionIdx = i;
            maxPriority = priority;
        }
    }

    gameState.applyAction(actions[actionIdx]);

    MCTSNode& child = children[actionCodes[actionIdx]];

    double reward = child.explore(gameState);
    totalReward += reward;

    return reward;
}

int MCTSNode::choseAction(GameState gameState)
{
    std::vector<int> actions = gameState.validActions();
    std::shuffle(actions.begin(), actions.end(), RNG);

    int numActions = actions.size();

    double maxVisits = 0;
    int actionIdx = numActions;

    for (int i = 0; i < numActions; ++i)
    {
        int actionCode = gameState.actionCode(actions[i]);
        int childVisits = children[actionCode].visits;

        if (i == 0 || childVisits > maxVisits)
        {
            actionIdx = i;
            maxVisits = childVisits;
        }
    }

    return actions[actionIdx];
}
