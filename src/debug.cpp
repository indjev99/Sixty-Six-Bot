#include "mcts.h"
#include "game_state.h"
#include "printing.h"
#include "util.h"
#include <iostream>

void printAction(int code)
{
    if (code == NUM_ACODES + M_CLOSE) std::cout << "Close";
    else if (code == NUM_ACODES + M_EXCHANGE) std::cout << "Exchange";
    else printCard(GameState::acodeCard(code));
}

std::string actionString(int code)
{
    if (code == NUM_ACODES + M_CLOSE) return "Close";
    else if (code == NUM_ACODES + M_EXCHANGE) return "Exchange";
    else return GameState::acodeCard(code).toString();
}

void MCTSNode::debug(GameState& gameState, bool selfRedetermed)
{
    std::cout << std::endl;

    if (gameState.isTerminal())
    {
        std::cout << "Result: " << gameState.result() << std::endl;
        std::cout << std::endl;
        return;
    }

    std::vector<int> actions = gameState.validActions();

    int numActions = actions.size();

    double maxVisits = 0;
    int actionIdx = numActions;

    int currPlayerMult = gameState.currentPlayer();

    std::cout << "Player: " << gameState.currentPlayer() << std::endl;

    bool nextSR = currPlayerMult > 0 ? selfRedetermed : false;

    for (int i = 0; i < numActions; ++i)
    {
        int actionCode = gameState.actionCode(actions[i]);
        int childVisits = children[actionCode] ? children[actionCode]->visits[nextSR] : 0;

        std::cout << " ";
        printAction(actionCode);
        std::cout << ": " << childVisits;
        if (childVisits > 0) std::cout << " " << children[actionCode]->totalReward[nextSR] / childVisits;
        std::cout << std::endl;

        if (childVisits > maxVisits)
        {
            actionIdx = i;
            maxVisits = childVisits;
        }
    }

    std::cout << std::endl;

    std::string actionStr;
    std::cout << "Choosing: ";
    std::cin >> actionStr;

    moveUpOneLine();

    if (stringMatch(actionStr, "Stop")) return;

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

    if (children[actionCode] == nullptr || children[actionCode]->visits[nextSR] == 0) return;

    std::cout << "Choosing: ";
    printAction(actionCode);
    std::cout << std::endl;

    gameState.applyAction(actions[actionIdx]);
    children[actionCode]->debug(gameState, selfRedetermed);
}
