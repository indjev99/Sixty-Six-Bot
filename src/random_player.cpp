#include "random_player.h"
#include "config.h"
#include "rng.h"

void RandomPlayer::reset() {}
void RandomPlayer::giveState(bool closed, int talonSize, Card trumpCard, int score, int oppScore) {}
void RandomPlayer::giveHand(const std::vector<Card>& hand) {}
void RandomPlayer::giveMove(Move move) {}
void RandomPlayer::giveResponse(Card card) {}
void RandomPlayer::giveGameResult(int points) {}

int RandomPlayer::getMove()
{
    return randInt(M_EXCHANGE, HAND_SIZE);
}

int RandomPlayer::getResponse(const std::vector<int>& valid)
{
    return valid[randInt(0, valid.size())];
}
