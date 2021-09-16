
#pragma once

#include "card.h"
#include "talon.h"
#include "player.h"
#include <vector>

struct PlayerGameState
{
    PlayerGameState();
    PlayerGameState(int mult, Player* player);

    int mult;
    Player* player;

    int score;
    bool hasTakenTricks;
    bool hasClosed;

    std::vector<Card> hand;
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

    void applyPlayerAction(int attempts = 1);
    int actionCode(int idx);

private:

    void updateMarriageSuits();

    int trumpSuit;
    int trickNumber;
    bool closed;
    Move move;
    PlayerGameState leadState, respState;
    Talon talon;
    int noActionPlayer;

    std::vector<bool> isMarriageSuit;
};
