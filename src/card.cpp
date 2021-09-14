#include "card.h"

const std::string rankNames[NUM_RANKS] = {"9", "J", "Q", "K", "10", "A"};
const std::string suitNames[NUM_SUITS] = {"s", "h", "c", "d"};

Card::Card() {}

Card::Card(int code):
    rank(code % NUM_RANKS),
    suit(code / NUM_RANKS) {}

Card::Card(int rank, int suit):
    rank(rank),
    suit(suit) {}

int Card::code() const
{
    return suit * NUM_RANKS + rank;
}

std::string Card::toString() const
{
    return rankNames[rank] + suitNames[suit];
}

bool operator==(Card left, Card right)
{
    return left.suit == right.suit && left.rank == right.rank;
}

bool operator<(Card left, Card right)
{
    if (left.suit != right.suit) return left.suit < right.suit;
    return left.rank < right.rank;
}
