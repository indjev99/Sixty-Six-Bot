#include "mcts.h"
#include "ismcts.h"
#include "game_state.h"
#include "printing.h"
#include "util.h"
#include <iostream>

void printAction(int code)
{
    if (code == M_CLOSE) std::cout << "Close";
    else if (code == M_EXCHANGE) std::cout << "Exchange";
    else printCard(Card(code % (NUM_SUITS * NUM_RANKS)));
}

std::string actionString(int code)
{
    if (code == M_CLOSE) return "Close";
    else if (code == M_EXCHANGE) return "Exchange";
    else return Card(code % (NUM_SUITS * NUM_RANKS)).toString();
}

void MCTSNode::debug(GameState gameState)
{
    std::cout << std::endl;

    if (visits == 0) return;

    std::vector<int> actions = gameState.validActions();

    int numActions = actions.size();

    double maxVisits = 0;
    int actionIdx = numActions;

    std::cout << "Player: " << gameState.currentPlayer() << std::endl;

    for (int i = 0; i < numActions; ++i)
    {
        int actionCode = gameState.actionCode(actions[i]);
        int childVisits = children[actionCode].visits;

        std::cout << " ";
        printAction(actionCode);
        std::cout << ": " << childVisits;
        if (childVisits > 0) std::cout << " " << children[actionCode].totalReward / childVisits;
        std::cout << std::endl;

        if (i == 0 || childVisits > maxVisits)
        {
            actionIdx = i;
            maxVisits = childVisits;
        }
    }

    std::cout << std::endl;

    std::string actionStr;
    std::cout << "Choosing: ";
    std::cin >> actionStr;

    if (stringMatch(actionStr, "Stop"))
    {
        std::cout << std::endl;
        return;
    }

    moveUpOneLine();

    for (int i = 0; i < numActions; ++i)
    {
        int actionCode = gameState.actionCode(actions[i]);
        if (stringMatch(actionStr, actionString(actionCode)))
        {
            actionIdx = i;
            break;
        }
    }

    int actionCode = gameState.actionCode(actions[actionIdx]);

    std::cout << "Choosing: ";
    printAction(actionCode);
    std::cout << std::endl;

    gameState.applyAction(actions[actionIdx]);
    children[actionCode].debug(gameState);
}

void ISMCTSNode::debug(GameState gameState, int currDeterm, int selfDeterm, int oppDeterm)
{
    std::cout << std::endl;

    if (visits[currDeterm] == 0) return;

    std::vector<int> actions = gameState.validActions();

    int numActions = actions.size();

    double maxVisits = 0;
    int actionIdx = numActions;

    int currPlayerMult = gameState.currentPlayer();

    std::cout << "Player: " << gameState.currentPlayer() << std::endl;

    int nextDeterm = currPlayerMult > 0 ? selfDeterm : oppDeterm;

    for (int i = 0; i < numActions; ++i)
    {
        int actionCode = gameState.actionCode(actions[i]);
        int childVisits = children[actionCode].visits[nextDeterm];

        std::cout << " ";
        printAction(actionCode);
        std::cout << ": " << childVisits;
        if (childVisits > 0) std::cout << " " << children[actionCode].totalReward[nextDeterm] / childVisits;
        std::cout << std::endl;

        if (i == 0 || childVisits > maxVisits)
        {
            actionIdx = i;
            maxVisits = childVisits;
        }
    }

    std::cout << std::endl;

    std::string actionStr;
    std::cout << "Choosing: ";
    std::cin >> actionStr;

    if (stringMatch(actionStr, "Stop"))
    {
        std::cout << std::endl;
        return;
    }

    moveUpOneLine();

    for (int i = 0; i < numActions; ++i)
    {
        int actionCode = gameState.actionCode(actions[i]);
        if (stringMatch(actionStr, actionString(actionCode)))
        {
            actionIdx = i;
            break;
        }
    }

    int actionCode = gameState.actionCode(actions[actionIdx]);

    std::cout << "Choosing: ";
    printAction(actionCode);
    std::cout << std::endl;

    gameState.applyAction(actions[actionIdx]);
    children[actionCode].debug(gameState, nextDeterm, selfDeterm, oppDeterm);
}
