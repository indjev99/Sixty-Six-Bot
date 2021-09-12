#include "player_simple.h"
#include "config.h"
#include "util.h"
#include "rng.h"
#include <algorithm>

void PlayerSimple::giveState(bool closed, int talonSize, Card trumpCard, int selfScore, int oppScore)
{
    this->closed = closed;
    this->talonSize = talonSize;
    if (trickNumber == 0) trumpSuite = trumpCard.suite;
}

void PlayerSimple::giveHand(const std::vector<Card>& hand)
{
    this->hand = hand;
}

void PlayerSimple::giveMove(Move move)
{
    leadCard = move.card;
}

void PlayerSimple::giveResponse(Card card)
{
    ++trickNumber;
}

void PlayerSimple::giveGameResult(int newPoints, int selfPoints, int oppPoints)
{
    trickNumber = 0;
}

bool isBetter(int trumpSuite, Card card, Card other)
{
    if ((card.suite == trumpSuite) != (other.suite == trumpSuite)) return card.suite == trumpSuite;
    return card.rank > other.rank;
}

int PlayerSimple::getMove()
{
    if (!closed && trickNumber > 0 && talonSize >= TALON_ACT_TRESH && findExchangeCard(trumpSuite, hand) < (int) hand.size()) return M_EXCHANGE;

    int targetMarriageSuite = -1;
    std::vector<bool> marriageSuites = findMarriageSuits(hand);
    for (int i = 0; i < NUM_SUITES; ++i)
    {
        if (marriageSuites[i]) targetMarriageSuite = i;
    }
    if (marriageSuites[trumpSuite]) targetMarriageSuite = trumpSuite;
    if (trickNumber > 0 && targetMarriageSuite != -1)
    {
        for (int i = 0; i < (int) hand.size(); ++i)
        {
            if (hand[i].suite == targetMarriageSuite && hand[i].rank == MARRIAGE_RANKS.front()) return i;
        }
    }

    int move = -1;
    bool targetBetter = closed || talonSize == 0;
    for (int i = 0; i < (int) hand.size(); ++i)
    {
        if (move == -1 || isBetter(trumpSuite, hand[i], hand[move]) == targetBetter) move = i;
    }

    return move;
}

int PlayerSimple::getResponse(const std::vector<int>& valid)
{
    std::vector<int> priorities(hand.size(), 0);
    std::vector<bool> marriageSuites = findMarriageSuits(hand);

    for (int i : valid)
    {
        if (hand[i].suite == trumpSuite) priorities[i] -= 5 + hand[i].rank;
        if (marriageSuites[hand[i].suite] && isMarriageCard(hand[i])) priorities[i] -= hand[i].suite != trumpSuite ? REG_MARRIAGE_VALUE : TRUMP_MARRIAGE_VALUE;
        if (!leadWinsTrick(trumpSuite, leadCard, hand[i])) priorities[i] += CARD_VALUES[leadCard.rank] + (hand[i].suite != trumpSuite) * CARD_VALUES[hand[i].suite];
        else priorities[i] -= CARD_VALUES[leadCard.rank] + CARD_VALUES[hand[i].suite];
    }

    int response = -1;
    for (int i : valid)
    {
        if (response == -1 || priorities[i] > priorities[response]) response = i;
    }

    ++trickNumber;
    return response;
}
