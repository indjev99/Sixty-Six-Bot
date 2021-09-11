#pragma once

#include "card.h"
#include "hand.h"
#include <vector>

struct Talon
{
    Talon();

    int size() const;
    std::pair<Hand, Hand> dealTwoHands();
    std::pair<Card, Card> dealTwoCards();

    Card lastCard() const;
    void setLastCard(Card card);

private:

    std::vector<Card> cards;
};
