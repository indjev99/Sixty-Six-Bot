#include "random_player.h"
#include "rng.h"

void RandomPlayer::giveState(bool closed, int talonSize, Card trumpCard, int selfScore, int oppScore) {}
void RandomPlayer::giveMove(Move move) {}
void RandomPlayer::giveResponse(Card card) {}
void RandomPlayer::giveGameResult(int newPoints, int selfPoints, int oppPoints) {}

void RandomPlayer::giveHand(const std::vector<Card>& hand)
{
    handSize = hand.size();
}

int RandomPlayer::getMove()
{
    return randInt(M_EXCHANGE, handSize);
}

int RandomPlayer::getResponse(const std::vector<int>& valid)
{
    return valid[randInt(0, valid.size())];
}
