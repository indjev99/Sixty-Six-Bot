#pragma once

#include "card.h"
#include "move.h"
#include <vector>

struct Player
{
    virtual void giveTrump(Card card) = 0;

    virtual void giveHand(const std::vector<Card>& hand) = 0;
    virtual void giveMove(Move move, bool self) = 0;
    virtual void giveResponse(Card card, bool self) = 0;

    virtual void giveGameResult(int points) = 0;

    virtual int getMove() = 0;
    virtual int getResponse() = 0;
};
