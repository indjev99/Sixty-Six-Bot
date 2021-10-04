#pragma once

#include "player.h"
#include "external_source.h"

int playGame(Player* leadPlayer, Player* respPlayer);
int playGameExternal(Player* player, ExternalSource* extSource, int leadingMult = 0);
