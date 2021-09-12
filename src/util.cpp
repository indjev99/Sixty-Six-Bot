#include "util.h"
#include "config.h"
#include <algorithm>
#include <numeric>

static std::string toLower(std::string str)
{
    for (char& c : str)
    {
        c = std::tolower(c);
    }
    return str;
}

bool stringMatch(const std::string& left, const std::string& right)
{
    return toLower(left) == toLower(right);
}

int findCard(const std::string& name, const std::vector<Card>& hand)
{
    for (int i = 0; i < (int) hand.size(); ++i)
    {
        if (stringMatch(hand[i].toString(), name)) return i;
    }
    return hand.size();
}

bool leadWinsTrick(int trumpSuit, Card leadCard, Card respCard)
{
    if (leadCard.suit != trumpSuit && respCard.suit == trumpSuit) return false;
    if (leadCard.suit == respCard.suit && leadCard.rank < respCard.rank) return false;
    return true;
}

int findExchangeCard(int trumpSuit, const std::vector<Card>& hand)
{
    for (int i = 0; i < (int) hand.size(); ++i)
    {
        if (hand[i].suit == trumpSuit && hand[i].rank == EXCHANGE_RANK) return i;
    }
    return hand.size();
}

bool isMarriageCard(Card card)
{
    return std::find(MARRIAGE_RANKS.begin(), MARRIAGE_RANKS.end(), card.rank) != MARRIAGE_RANKS.end();
}

std::vector<bool> findMarriageSuits(const std::vector<Card>& hand)
{
    std::vector<int> marriageCounts(NUM_SUITS, 0);
    for (int i = 0; i < (int) hand.size(); ++i)
    {
        if (isMarriageCard(hand[i])) ++marriageCounts[hand[i].suit];
    }

    std::vector<bool> marriageSuits(NUM_SUITS);
    for (int i = 0; i < NUM_SUITS; ++i)
    {
        marriageSuits[i] = marriageCounts[i] == (int) MARRIAGE_RANKS.size();
    }

    return marriageSuits;
}

std::vector<int> findValidResponses(int trumpSuit, Card leadCard, const std::vector<Card>& hand)
{
    std::vector<int> suitdRaises;
    std::vector<int> suitd;
    std::vector<int> trumps;

    for (int i = 0; i < (int) hand.size(); ++i)
    {
        if (hand[i].suit == trumpSuit) trumps.push_back(i);
        if (hand[i].suit == leadCard.suit) suitd.push_back(i);
        if (hand[i].suit == leadCard.suit && hand[i].rank > leadCard.rank) suitdRaises.push_back(i);
    }

    if (!suitdRaises.empty()) return suitdRaises;
    else if (!suitd.empty()) return suitd;
    else if (!trumps.empty()) return trumps;

    std::vector<int> valid(hand.size());
    std::iota(valid.begin(), valid.end(), 0);
    return valid;
}