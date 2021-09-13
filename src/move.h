#pragma once

#include "card.h"

#define M_PLAY 0
#define M_CLOSE -1
#define M_EXCHANGE -2
#define M_NONE -10

struct Move
{
    int type;
    Card card;
    int score = 0;
};
