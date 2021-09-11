#include "util.h"
#include "config.h"
#include <algorithm>

bool leadWinsTrick(int trump, Card leadCard, Card respCard)
{
    if (leadCard.suite != trump && respCard.suite == trump) return false;
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

int findExchangeCard(int trump, const std::vector<Card>& hand)
{
    for (int i = 0; i < (int) hand.size(); ++i)
    {
        if (hand[i].suite == trump && hand[i].rank == EXCHANGE_RANK) return i;
    }
    return hand.size();
}

std::vector<bool> findMarriageSuits(const std::vector<Card>& hand)
{
    std::vector<int> marriageCounts(NUM_SUITES, 0);
    for (int i = 0; i < (int) hand.size(); ++i)
    {
        if (std::find(MARRIAGE_RANKS.begin(), MARRIAGE_RANKS.end(), hand[i].rank) != MARRIAGE_RANKS.end()) ++marriageCounts[hand[i].suite];
    }

    std::vector<bool> marriageSuites(NUM_SUITES);
    for (int i = 0; i < NUM_SUITES; ++i)
    {
        marriageSuites[i] = marriageCounts[i] == (int) MARRIAGE_RANKS.size();
    }

    return marriageSuites;
}