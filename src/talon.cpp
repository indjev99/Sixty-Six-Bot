#include "talon.h"
#include "config.h"
#include "rng.h"
#include <algorithm>

Talon::Talon()
{
    for (int rank = 0; rank < NUM_RANKS; ++rank)
    {
        for (int suit = 0; suit < NUM_SUITES; ++suit)
        {
            cards.push_back({rank, suit});
        }
    }

    std::shuffle(cards.begin(), cards.end(), RNG);
}

int Talon::size() const
{
    return cards.size();
}

std::vector<Card> Talon::dealHand()
{
    std::vector<Card> hand(cards.end() - HAND_SIZE, cards.end());
    cards.resize(cards.size() - HAND_SIZE);
    return hand;
}

Card Talon::dealCard()
{
    Card card = cards.back();
    cards.pop_back();
    return card;
}

Card Talon::lastCard() const
{
    if (cards.empty()) return {-1, -1};
    return cards.front();
}

void Talon::setLastCard(Card card)
{
    cards.front() = card;
}
