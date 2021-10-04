#pragma once

#include <string>

#define NUM_RANKS 6
#define R_NINE 0
#define R_JACK 1
#define R_QUEEN 2
#define R_KING 3
#define R_TEN 4
#define R_ACE 5

#define NUM_SUITS 4
#define S_CLUBS 0
#define S_DIAMONDS 1
#define S_SPADES 2
#define S_HEARTS 3

struct Card
{
    int rank;
    int suit;

    Card();
    Card(int code);
    Card(int rank, int suit);

    int code() const;
    std::string toString() const;
};

bool operator==(Card left, Card right);
bool operator<(Card left, Card right);
