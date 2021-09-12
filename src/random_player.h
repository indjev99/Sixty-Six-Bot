#pragma once

#include "player.h"

struct RandomPlayer : Player
{
    void reset() override;

    void giveState(bool closed, int talonSize, Card trumpCard, int score, int oppScore) override;
    void giveHand(const std::vector<Card>& hand) override;
    void giveMove(Move move) override;
    void giveResponse(Card card) override;

    void giveGameResult(int points) override;

    int getMove() override;
    int getResponse() override;
};
