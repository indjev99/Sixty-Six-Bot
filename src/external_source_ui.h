#pragma once

#include "external_source.h"

struct ExternalSourceUI : ExternalSource
{
    ExternalSourceUI(bool printing);

    bool getLeading() override;
    Card getTrumpCard() override;
    std::vector<Card> getHand() override;
    Card getDrawnCard() override;
    Move getMove() override;
    void giveMove(Move move) override;

private:

    bool printing;
};
