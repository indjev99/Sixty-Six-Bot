#pragma once

#include "card.h"
#include <string>

#define NUM_SPEC_MOVES 2
#define M_PLAY 0
#define M_CLOSE -1
#define M_EXCHANGE -2
#define M_NONE -10

struct Move
{
    int type;
    Card card;
    int score;

    Move(int type = M_NONE, Card card = Card(-1, -1), int score = 0);
    std::string toString() const;
};
