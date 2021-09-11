#include "card.h"

const std::string rankNames[NUM_RANKS] = {"9", "J", "Q", "K", "10", "A"};
const std::string suiteNames[NUM_SUITES] = {"s", "h", "c", "d"};

std::string Card::toString() const
{
    return rankNames[rank] + suiteNames[suite];
}

bool operator<(Card left, Card right)
{
    if (left.suite != right.suite) return left.suite < right.suite;
    return left.rank < right.rank;
}