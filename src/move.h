#pragma once

#include "card.h"

#define NUM_SPEC_MOVES 2
#define M_PLAY 0
#define M_CLOSE -1
#define M_EXCHANGE -2
#define M_NONE -10

struct Move
{
    int type = M_NONE;
    Card card;
    int score = 0;
};
