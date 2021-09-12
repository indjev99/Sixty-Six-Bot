#include "card.h"

const std::string rankNames[NUM_RANKS] = {"9", "J", "Q", "K", "10", "A"};
const std::string suitNames[NUM_SUITES] = {"s", "h", "c", "d"};

std::string Card::toString() const
{
    return rankNames[rank] + suitNames[suit];
}

bool operator<(Card left, Card right)
{
    if (left.suit != right.suit) return left.suit < right.suit;
    return left.rank < right.rank;
}