#include "talon.h"
#include "config.h"
#include "rng.h"
#include <algorithm>
#include <assert.h>

Talon::Talon()
{
    for (int code = 0; code < NUM_RANKS * NUM_SUITS; ++code)
    {
        cards.push_back(Card(code));
    }

    std::shuffle(cards.begin(), cards.end(), RNG);
}

Talon::Talon(const std::vector<Card>& cards):
    cards(cards) {}

int Talon::size() const
{
    return cards.size();
}

std::vector<Card> Talon::dealHand()
{
    assert((int) cards.size() >= HAND_SIZE);
    std::vector<Card> hand(cards.end() - HAND_SIZE, cards.end());
    cards.resize(cards.size() - HAND_SIZE);
    return hand;
}

Card Talon::drawCard()
{
    assert(!cards.empty());
    Card card = cards.back();
    cards.pop_back();
    return card;
}

Card Talon::lastCard() const
{
    assert(!cards.empty());
    return cards.front();
}

void Talon::setLastCard(Card card)
{
    assert(!cards.empty());
    cards.front() = card;
}
