#include "play_game.h"
#include "game_state.h"
#include "config.h"
#include "util.h"
#include <algorithm>
#include <iostream>
#include <assert.h>

int playGameExternal(Player* player, ExternalSource* extSource, int leadingMult)
{
    if (leadingMult == 0) leadingMult = extSource->getLeading() ? 1 : -1;
    GameState gameState = leadingMult == 1 ? GameState(player, nullptr) : GameState(nullptr, player);

    Card trumpCard = extSource->getTrumpCard();
    gameState.extSetTrumpCard(trumpCard);

    std::vector<Card> hand = extSource->getHand();
    gameState.extSetHand(hand, leadingMult);

    bool drawCard = false;
    while (!gameState.isTerminal())
    {
        if (drawCard)
        {
            Card drawnCard = extSource->getDrawnCard();
            gameState.extSetDrawnCard(drawnCard, leadingMult);
        }

        if (gameState.currentPlayer() == leadingMult)
        {
            std::pair<Move, bool> moveDraw = gameState.extApplyPlayerAction(leadingMult);
            extSource->giveMove(moveDraw.first);
            drawCard = moveDraw.second;
        }
        else
        {
            Move move = extSource->getMove();
            int handSize = gameState.extGetHandSize(-leadingMult);

            hand.clear();
            if (move.type == M_EXCHANGE) hand.push_back(Card(EXCHANGE_RANK, trumpCard.suit));
            else if (move.type == M_PLAY)
            {
                hand.push_back(move.card);
                if (move.score)
                {
                    for (int rank : MARRIAGE_RANKS)
                    {
                        if (rank == move.card.rank) continue;
                        hand.push_back(Card(rank, move.card.suit));
                    }
                }
            }
            while ((int) hand.size() < handSize)
            {
                hand.push_back(Card(-1, -1));
            }

            gameState.extSetHand(hand, -leadingMult);
            std::vector<int> valid = gameState.validActions();

            int idx = move.type == M_PLAY ? 0 : move.type;
            assert(std::find(valid.begin(), valid.end(), idx) != valid.end());
            drawCard = gameState.extApplyAction(idx, leadingMult);
        }
    }

    return gameState.result() * leadingMult;
}
