#pragma once

#define NUM_RANKS 6
#define R_NINE 0
#define R_JACK 1
#define R_QUEEN 2
#define R_KING 3
#define R_TEN 4
#define R_ACE 5

#define NUM_SUITES 4
#define S_SPADES 0
#define S_HEARTS 1
#define S_CLUBS 2
#define S_DIAMONDS 3

#include <string>

struct Card
{
    int rank;
    int suite;

    std::string toString() const;
};

bool operator<(Card left, Card right);