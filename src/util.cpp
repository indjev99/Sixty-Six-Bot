#include "util.h"
#include "config.h"
#include <algorithm>
#include <numeric>

bool leadWinsTrick(int trumpSuite, Card leadCard, Card respCard)
{
    if (leadCard.suite != trumpSuite && respCard.suite == trumpSuite) return false;
    if (leadCard.suite == respCard.suite && leadCard.rank < respCard.rank) return false;
    return true;
}

int findCard(std::string name, const std::vector<Card>& hand)
{
    for (int i = 0; i < (int) hand.size(); ++i)
    {
        if (hand[i].toString() == name) return i;
    }
    return hand.size();
}

int findExchangeCard(int trumpSuite, const std::vector<Card>& hand)
{
    for (int i = 0; i < (int) hand.size(); ++i)
    {
        if (hand[i].suite == trumpSuite && hand[i].rank == EXCHANGE_RANK) return i;
    }
    return hand.size();
}

bool isMarriageCard(Card card)
{
    return std::find(MARRIAGE_RANKS.begin(), MARRIAGE_RANKS.end(), card.rank) != MARRIAGE_RANKS.end();
}

std::vector<bool> findMarriageSuits(const std::vector<Card>& hand)
{
    std::vector<int> marriageCounts(NUM_SUITES, 0);
    for (int i = 0; i < (int) hand.size(); ++i)
    {
        if (isMarriageCard(hand[i])) ++marriageCounts[hand[i].suite];
    }

    std::vector<bool> marriageSuites(NUM_SUITES);
    for (int i = 0; i < NUM_SUITES; ++i)
    {
        marriageSuites[i] = marriageCounts[i] == (int) MARRIAGE_RANKS.size();
    }

    return marriageSuites;
}

std::vector<int> findValidResponses(int trumpSuite, Card leadCard, const std::vector<Card>& hand)
{
    std::vector<int> suitedRaises;
    std::vector<int> suited;
    std::vector<int> trumps;

    for (int i = 0; i < (int) hand.size(); ++i)
    {
        if (hand[i].suite == trumpSuite) trumps.push_back(i);
        if (hand[i].suite == leadCard.suite) suited.push_back(i);
        if (hand[i].suite == leadCard.suite && hand[i].rank > leadCard.rank) suitedRaises.push_back(i);
    }

    if (!suitedRaises.empty()) return suitedRaises;
    else if (!suited.empty()) return suited;
    else if (!trumps.empty()) return trumps;

    std::vector<int> valid(hand.size());
    std::iota(valid.begin(), valid.end(), 0);
    return valid;
}