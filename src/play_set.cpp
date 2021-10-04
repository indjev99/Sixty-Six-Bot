#include "play_set.h"
#include "play_game.h"
#include "config.h"

struct PlayerSetState
{
    int mult;
    Player* player;

    int points = 0;
};

int playSetCombined(Player* leadPlayer, Player* respPlayer, ExternalSource* extSource)
{
    PlayerSetState leadState, respState;

    leadState.mult = 1;
    leadState.player = leadPlayer;

    respState.mult = -1;
    respState.player = respPlayer;

    if (leadState.player) leadState.player->startSet();
    if (respState.player) respState.player->startSet();

    if (extSource && !extSource->getLeading()) std::swap(leadState, respState);

    while (respState.points < POINT_TRESH)
    {
        int newPoints;
        if (extSource)
        {
            if (leadState.player) newPoints = playGameExternal(leadState.player, extSource, 1);
            else newPoints = -playGameExternal(respState.player, extSource, -1);
        }
        else newPoints = playGame(leadState.player, respState.player);

        if (newPoints >= 0) std::swap(leadState, respState);
        newPoints = std::abs(newPoints);

        respState.points += newPoints;

        if (leadState.player) leadState.player->giveGameResult(-newPoints, leadState.points, respState.points);
        if (respState.player) respState.player->giveGameResult(newPoints, respState.points, leadState.points);
    }

    if (leadState.player) leadState.player->giveSetResult(-1);
    if (respState.player) respState.player->giveSetResult(1);

    return respState.mult;
}

int playSet(Player* leadPlayer, Player* respPlayer)
{
    return playSetCombined(leadPlayer, respPlayer, nullptr);
}

int playSetExternal(Player* player, ExternalSource* extSource)
{
    return playSetCombined(player, nullptr, extSource);
}