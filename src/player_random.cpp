#include "player_random.h"
#include "rng.h"

void PlayerRandom::startSet() {}
void PlayerRandom::startGame() {}
void PlayerRandom::giveState(bool closed, int talonSize, Card trumpCard, int selfScore, int oppScore) {}
void PlayerRandom::giveHand(const std::vector<Card>& hand) {}
void PlayerRandom::giveMove(Move move, bool self) {}
void PlayerRandom::giveResponse(Card card, bool self) {}
void PlayerRandom::giveGameResult(int newPoints, int selfPoints, int oppPoints) {}
void PlayerRandom::giveSetResult(int result) {}

int PlayerRandom::getMove(const std::vector<int>& valid)
{
    return valid[randInt(0, valid.size())];
}

int PlayerRandom::getResponse(const std::vector<int>& valid)
{
    return valid[randInt(0, valid.size())];
}
