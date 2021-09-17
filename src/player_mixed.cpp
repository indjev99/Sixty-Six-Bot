#include "player_mixed.h"

PlayerMixed::PlayerMixed(int switchTrick, Player* player1, Player* player2):
    switchTrick(switchTrick),
    player1(player1),
    player2(player2) {}

void PlayerMixed::startSet()
{
    player1->startSet();
    player2->startSet();
}

void PlayerMixed::startGame()
{
    player1->startGame();
    player2->startGame();

    trickNumber = 0;
}

void PlayerMixed::giveState(bool closed, int talonSize, Card trumpCard, int selfScore, int oppScore)
{
    player1->giveState(closed, talonSize, trumpCard, selfScore, oppScore);
    player2->giveState(closed, talonSize, trumpCard, selfScore, oppScore);
}

void PlayerMixed::giveHand(const std::vector<Card>& hand)
{
    player1->giveHand(hand);
    player2->giveHand(hand);
}

void PlayerMixed::giveMove(Move move, bool self)
{
    player1->giveMove(move, self);
    player2->giveMove(move, self);
}

void PlayerMixed::giveResponse(Card card, bool self)
{
    player1->giveResponse(card, self);
    player2->giveResponse(card, self);

    ++trickNumber;
}

void PlayerMixed::giveGameResult(int newPoints, int selfPoints, int oppPoints)
{
    player1->giveGameResult(newPoints, selfPoints, oppPoints);
    player2->giveGameResult(newPoints, selfPoints, oppPoints);
}

void PlayerMixed::giveSetResult(int result)
{
    player1->giveSetResult(result);
    player2->giveSetResult(result);
}

int PlayerMixed::getMove(const std::vector<int>& valid)
{
    Player* currPlayer = trickNumber < switchTrick ? player1 : player2;
    return currPlayer->getMove(valid);
}

int PlayerMixed::getResponse(const std::vector<int>& valid)
{
    Player* currPlayer = trickNumber < switchTrick ? player1 : player2;
    return currPlayer->getResponse(valid);
}
