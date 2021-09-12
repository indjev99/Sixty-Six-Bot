#pragma once

#include "card.h"
#include "move.h"
#include <vector>

struct Player
{
    virtual void startSet() = 0;
    virtual void startGame() = 0;

    virtual void giveState(bool closed, int talonSize, Card trumpCard, int selfScore, int oppScore) = 0;
    virtual void giveHand(const std::vector<Card>& hand) = 0;
    virtual void giveMove(Move move) = 0;
    virtual void giveResponse(Card card) = 0;

    virtual void giveGameResult(int newPoints, int selfPoints, int oppPoints) = 0;
    virtual void giveSetResult(int result) = 0;

    virtual int getMove() = 0;
    virtual int getResponse(const std::vector<int>& valid) = 0;
};
