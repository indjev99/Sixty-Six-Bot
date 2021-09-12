#pragma once

#include "card.h"
#include "move.h"
#include <vector>

struct Player
{
    virtual void reset() = 0;

    virtual void giveState(bool closed, int talonSize, Card trumpCard, int score, int oppScore) = 0;
    virtual void giveHand(const std::vector<Card>& hand) = 0;
    virtual void giveMove(Move move) = 0;
    virtual void giveResponse(Card card) = 0;

    virtual void giveGameResult(int points) = 0;

    virtual int getMove() = 0;
    virtual int getResponse() = 0;
};
