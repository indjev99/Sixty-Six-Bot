#include "player_simple.h"
#include "config.h"
#include "util.h"
#include "rng.h"

void PlayerSimple::giveState(bool closed, int talonSize, Card trumpCard, int selfScore, int oppScore)
{
    this->closed = closed;
    this->talonSize = talonSize;
    if (trickNumber == 0) trumpSuite = trumpCard.suite;
}

void PlayerSimple::giveHand(const std::vector<Card>& hand)
{
    this->hand = hand;
}

void PlayerSimple::giveMove(Move move)
{
    leadCard = move.card;
}

void PlayerSimple::giveResponse(Card card)
{
    ++trickNumber;
}

void PlayerSimple::giveGameResult(int newPoints, int selfPoints, int oppPoints)
{
    trickNumber = 0;
}

int PlayerSimple::getMove()
{
    if (!closed && trickNumber > 0 && talonSize >= TALON_ACT_TRESH && findExchangeCard(trumpSuite, hand) < (int) hand.size()) return M_EXCHANGE;
    int move = randInt(0, hand.size());
    return move;
}

int PlayerSimple::getResponse(const std::vector<int>& valid)
{
    int response = valid[randInt(0, valid.size())];
    ++trickNumber;
    return response;
}
