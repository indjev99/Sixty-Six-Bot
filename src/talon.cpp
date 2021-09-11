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

std::pair<Hand, Hand> Talon::dealTwoHands()
{
    std::vector<Card> hand1(cards.end() - HAND_SIZE, cards.end());
    std::vector<Card> hand2(cards.end() - 2 * HAND_SIZE, cards.end() - HAND_SIZE);
    cards.resize(cards.size() - 2 * HAND_SIZE);
    return {{hand1}, {hand2}};
}

std::pair<Card, Card> Talon::dealTwoCards()
{
    Card card1 = *(cards.end() - 1);
    Card card2 = *(cards.end() - 2);
    cards.resize(cards.size() - 2);
    return {card1, card2};
}

Card Talon::lastCard() const
{
    return cards.front();
}

void Talon::setLastCard(Card card)
{
    cards.front() = card;
}
