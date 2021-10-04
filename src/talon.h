#pragma once

#include "card.h"
#include <vector>

struct Talon
{
    Talon();
    Talon(const std::vector<Card>& cards);

    int size() const;
    std::vector<Card> dealHand();
    Card drawCard();

    Card lastCard() const;
    void setLastCard(Card card);

private:

    std::vector<Card> cards;
};
