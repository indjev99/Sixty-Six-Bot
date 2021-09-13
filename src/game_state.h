
#include "card.h"
#include "talon.h"
#include "player.h"
#include <vector>

struct PlayerGameState
{
    int mult;
    Player* player;

    std::vector<Card> hand;
    int score;
    bool hasTakenTricks;
    bool hasClosed;
};

struct GameState
{
    Talon talon;
    bool closed;
    int trickNumber;
    int trumpSuit;

    Move move;
    PlayerGameState leadState, respState;

    int noActionPlayer;
    bool giveInfo;

    GameState(Player* leadPlayer, Player* respPlayer);

    std::vector<int> validActions();
    void applyAction(int idx);
    bool isTerminal();
    int result();

    void applyPlayerAction(int attempts);
};
