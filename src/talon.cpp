#include "talon.h"
#include "config.h"
#include "rng.h"
#include <algorithm>

Talon::Talon()
{
    for (int rank = 0; rank < NUM_RANKS; ++rank)
    {
        for (int suite = 0; suite < NUM_SUITES; ++suite)
        {
            cards.push_back({rank, suite});
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
    return cards.front();
}

void Talon::setLastCard(Card card)
{
    cards.front() = card;
}
