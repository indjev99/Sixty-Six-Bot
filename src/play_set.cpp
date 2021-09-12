#include "play_set.h"
#include "play_game.h"
#include "config.h"

struct PlayerSetState
{
    int mult;
    Player* player;

    int points = 0;
};

int playSet(Player* leadPlayer, Player* respPlayer)
{
    PlayerSetState leadState, respState;

    leadState.mult = 1;
    leadState.player = leadPlayer;

    respState.mult = -1;
    respState.player = respPlayer;

    while (respState.points < POINT_TRESH)
    {
        int newPoints = playGame(leadState.player, respState.player);

        if (newPoints >= 0) std::swap(leadState, respState); 
        newPoints = std::abs(newPoints);

        respState.points += newPoints;

        leadState.player->giveGameResult(-newPoints, leadState.points, respState.points);
        respState.player->giveGameResult(newPoints, respState.points, leadState.points);
    }

    return respState.mult;
}