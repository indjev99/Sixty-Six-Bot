#pragma once

#include "player.h"

struct PlayerMixed : Player
{
    PlayerMixed(int switchTrick, Player* player1, Player* player2);

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

    int switchTrick;
    Player* player1;
    Player* player2;

    int trickNumber;
};
