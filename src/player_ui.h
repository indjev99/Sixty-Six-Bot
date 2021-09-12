#pragma once

#include "player.h"

struct PlayerUI : Player
{
    PlayerUI();
    PlayerUI(Player* player);

    void startSet() override;
    void startGame() override;

    void giveState(bool closed, int talonSize, Card trumpCard, int selfScore, int oppScore) override;
    void giveHand(const std::vector<Card>& hand) override;
    void giveMove(Move move) override;
    void giveResponse(Card card) override;

    void giveGameResult(int newPoints, int selfPoints, int oppPoints) override;
    void giveSetResult(int result) override;

    int getMove() override;
    int getResponse(const std::vector<int>& valid) override;

private:

    Player* player;

    int trumpSuit;
    int trickNumber;
    std::vector<Card> hand;
};
