#include "mcts.h"
#include "game_state.h"
#include "printing.h"
#include <iostream>

void printAction(int code)
{
    if (code == M_CLOSE) std::cout << "Close";
    else if (code == M_EXCHANGE) std::cout << "Exchange";
    else printCard(Card(code));
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

    int actionCode = gameState.actionCode(actions[actionIdx]);

    std::cout << std::endl;

    std::cout << "Choosing: ";
    printAction(actionCode);
    std::cout << std::endl;

    gameState.applyAction(actions[actionIdx]);
    children[actionCode].debug(gameState);
}
