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

bool isMarriageCard(int rank)
{
    return std::find(MARRIAGE_RANKS.begin(), MARRIAGE_RANKS.end(), rank) != MARRIAGE_RANKS.end();
}

std::vector<bool> findMarriageSuits(const std::vector<Card>& hand)
{
    std::vector<int> marriageCardCounts(NUM_SUITS, 0);
    for (int i = 0; i < (int) hand.size(); ++i)
    {
        if (isMarriageCard(hand[i].rank)) ++marriageCardCounts[hand[i].suit];
    }

    std::vector<bool> isMarriageSuit(NUM_SUITS);
    for (int i = 0; i < NUM_SUITS; ++i)
    {
        isMarriageSuit[i] = marriageCardCounts[i] == (int) MARRIAGE_RANKS.size();
    }

    return isMarriageSuit;
}
