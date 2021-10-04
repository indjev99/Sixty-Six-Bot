#pragma once

#include "card.h"
#include "move.h"
#include <vector>

struct ExternalSource
{
    virtual bool getLeading() = 0;
    virtual Card getTrumpCard() = 0;
    virtual std::vector<Card> getHand() = 0;
    virtual Card getDrawnCard() = 0;
    virtual Move getMove() = 0;
    virtual void giveMove(Move move) = 0;
};
