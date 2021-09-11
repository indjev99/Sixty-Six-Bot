#pragma once

#include "player.h"

struct HumanPlayer : Player
{
    void giveTrump(Card card) override;

    void giveHand(const std::vector<Card>& hand) override;
    void giveMove(Move move, bool self) override;
    void giveResponse(Card card, bool self) override;

    void giveGameResult(int points) override;

    int getMove() override;
    int getResponse() override;
};
