
#pragma once

#include "card.h"
#include "talon.h"
#include "player.h"
#include <vector>

struct PlayerGameState
{
    PlayerGameState(int mult = 0, Player* player = nullptr);

    void setHand(const std::vector<Card>& hand);
    void addCard(Card card);
    void removeCard(int idx);

    int mult;
    Player* player;

    int score;
    bool hasTakenTricks;
    bool hasClosed;

    std::vector<Card> hand;

    std::vector<bool> isMarriageSuit;
    std::vector<int> marriageCardCounts;
};

struct CardAnnotated
{
    Card card;
    bool canMarry;
    bool canExchange;
};

struct GameState
{
    GameState(Player* leadPlayer, Player* respPlayer);
    GameState(int trumpSuit, int trickNumber, bool closed, Move move, const PlayerGameState& leadState,
              const PlayerGameState& respState, const std::vector<Card>& talon);

    void setPlayers(Player* leadPlayer, Player* respPlayer);

    int currentPlayer();
    std::vector<int> validActions();
    void applyAction(int idx);
    bool isTerminal();
    int result();

    int playToTerminal(int attempts = 1);
    int actionCode(int idx) const;

    std::vector<int> recommendedActions();

    void reserveMem();

private:

    int trumpSuit;
    int trickNumber;
    bool closed;
    Move move;
    PlayerGameState leadState, respState;
    Talon talon;

    std::vector<int> tmpValid;
    std::vector<int> tmpSuitedRaises;
    std::vector<int> tmpSuited;
    std::vector<int> tmpTrumps;
    std::vector<int> tmpRecommended;
    std::vector<CardAnnotated> tmpLowest;
};
