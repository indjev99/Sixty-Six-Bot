#include "game_state.h"
#include "util.h"
#include <algorithm>

void GameState::extSetTrumpCard(Card trumpCard)
{
    trumpSuit = trumpCard.suit;
    talon.setLastCard(trumpCard);
}

void GameState::extSetHand(const std::vector<Card>& hand, int targetMult)
{
    PlayerGameState& targetState = leadState.mult == targetMult ? leadState : respState;
    targetState.setHand(hand);
}

void GameState::extSetDrawnCard(Card drawnCard, int targetMult)
{
    PlayerGameState& targetState = leadState.mult == targetMult ? leadState : respState;
    targetState.removeCard(targetState.hand.size() - 1);
    targetState.addCard(drawnCard);
}

int GameState::extGetHandSize(int targetMult)
{
    PlayerGameState& targetState = leadState.mult == targetMult ? leadState : respState;
    return targetState.hand.size();
}

bool GameState::extApplyAction(int idx, int targetMult)
{
    bool drawCard = false;
    if (move.type != M_NONE && !closed && talon.size() > 0)
    {
        bool leadWins = leadWinsTrick(trumpSuit, move.card, respState.hand[idx]);
        bool targetLeads = leadState.mult == targetMult;
        drawCard = talon.size() > 2 || (leadWins == targetLeads);
    }
    applyAction(idx);
    return drawCard;
}

std::pair<Move, bool> GameState::extApplyPlayerAction(int targetMult)
{
    validActions();

    int idx = HAND_SIZE;
    PlayerGameState& currState = move.type == M_NONE ? leadState : respState;
    while (std::find(tmpValid.begin(), tmpValid.end(), idx) == tmpValid.end())
    {
        if (move.type == M_NONE) idx = currState.player->getMove(tmpValid);
        else idx = currState.player->getResponse(tmpValid);
    }

    Move currMove;
    if (idx >= 0)
    {
        currMove.type = M_PLAY;
        currMove.card = currState.hand[idx];
        currMove.score = move.type == M_NONE && trickNumber > 0 && currState.isMarriageSuit[currMove.card.suit] && isMarriageRank(currMove.card.rank);
    }
    else currMove.type = idx;

    return {currMove, extApplyAction(idx, targetMult)};
}
