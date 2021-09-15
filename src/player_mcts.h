#pragma once

#include "player.h"
#include "game_state.h"

struct PlayerMCTS : Player
{
    PlayerMCTS(int numPlayouts, int numDeterms = 10, bool countCards = true);

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

    int numPlayouts;
    int numDeterms;

    bool countCards;

    void setCardKnownOpp(Card card);
    GameState determinize();
    int getAction();

    int trumpSuit;
    int trickNumber;
    bool closed;
    Move lastMove;
    
    PlayerGameState selfState, oppState;

    std::vector<Card> hand;

    int talonSize;
    Card lastTrumpCard;
    std::vector<Card> unseenCards;
    std::vector<Card> knownOppCards;
    std::vector<Card> knownTalonCards;
};
