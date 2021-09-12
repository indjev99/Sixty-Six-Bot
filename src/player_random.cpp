#include "player_random.h"
#include "rng.h"

void PlayerRandom::giveState(bool closed, int talonSize, Card trumpCard, int selfScore, int oppScore) {}
void PlayerRandom::giveMove(Move move) {}
void PlayerRandom::giveResponse(Card card) {}
void PlayerRandom::giveGameResult(int newPoints, int selfPoints, int oppPoints) {}

void PlayerRandom::giveHand(const std::vector<Card>& hand)
{
    handSize = hand.size();
}

int PlayerRandom::getMove()
{
    return randInt(M_EXCHANGE, handSize);
}

int PlayerRandom::getResponse(const std::vector<int>& valid)
{
    return valid[randInt(0, valid.size())];
}
