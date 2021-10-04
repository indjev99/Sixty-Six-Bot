#pragma once

#include "player.h"

struct PlayerUI : Player
{
    PlayerUI(Player* player = nullptr, bool pause = true);

    void startSet() override;
    void startGame() override;

    void giveState(bool closed, int talonSize, Card trumpCard, int selfScore, int oppScore) override;
    void giveHand(const std::vector<Card>& hand) override;
    void giveMove(Move move, bool self) override;
    void giveResponse(Card card, bool self) override;

    void giveGameResult(int newPoints, int selfPoints, int oppPoints) override;
    void giveSetResult(int result) override;

    int getMove(const std::vector<int>& valid) override;
    int getResponse(const std::vector<int>& valid) override;

private:

    Player* player;
    bool pause;

    int trumpSuit;
    bool closed;
    int talonSize;
    int trickNumber;
    std::vector<Card> hand;
};
