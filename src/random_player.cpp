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
    int move = randInt(M_EXCHANGE, HAND_SIZE * 2);
    if (move >= 0) return move % HAND_SIZE;
    else return move;
}

int RandomPlayer::getResponse()
{
    return randInt(0, HAND_SIZE);
}
