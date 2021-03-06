#pragma once

#include "card.h"
#include <vector>

const int HAND_SIZE = 6;
const int TALON_ACT_TRESH = 4;
const int EXCHANGE_RANK = R_NINE;
const int NUM_MARRIAGE_RANKS = 2;
const int MARRIAGE_RANKS[NUM_MARRIAGE_RANKS] = {R_QUEEN, R_KING};

const int CARD_VALUES[NUM_RANKS] = {0, 2, 3, 4, 10, 11};
const int REG_MARRIAGE_VALUE = 20;
const int TRUMP_MARRIAGE_VALUE = 40;
const int LAST_TRICK_VALUE = 10;

const int WIN_TRESH = 66;
const int BIGWIN_OPP_TRESH = 33;

const int WIN_POINTS = 1;
const int BIGWIN_POINTS = 2;
const int VALATWIN_POINTS = 3;
const int CLOSEFAIL_POINTS = 3;
const int NOACTION_POINTS = 20;

const int POINT_TRESH = 11;
