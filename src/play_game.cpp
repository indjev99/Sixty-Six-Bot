#include "play_game.h"
#include "game_state.h"
#include "talon.h"
#include "util.h"
#include "config.h"
#include <algorithm>
#include <numeric>

const int MAX_ATTEMPTS = 20;

int playGame(Player* leadPlayer, Player* respPlayer)
{
    GameState gameState(leadPlayer, respPlayer);
    while (!gameState.isTerminal())
    {
        gameState.applyPlayerAction(MAX_ATTEMPTS);
    }
    return gameState.result();
}
