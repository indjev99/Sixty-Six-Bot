#include "player_simple.h"
#include "config.h"
#include "util.h"
#include "rng.h"
#include <algorithm>

void PlayerSimple::startSet() {}

void PlayerSimple::startGame()
{
    trickNumber = 0;
}

void PlayerSimple::giveState(bool closed, int talonSize, Card trumpCard, int selfScore, int oppScore)
{
    this->closed = closed;
    this->talonSize = talonSize;
    if (trickNumber == 0) trumpSuit = trumpCard.suit;
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

void PlayerSimple::giveGameResult(int newPoints, int selfPoints, int oppPoints) {}
void PlayerSimple::giveSetResult(int result) {}

bool isBetter(int trumpSuit, Card card, Card other)
{
    if ((card.suit == trumpSuit) != (other.suit == trumpSuit)) return card.suit == trumpSuit;
    return card.rank > other.rank;
}

int PlayerSimple::getMove()
{
    if (!closed && trickNumber > 0 && talonSize >= TALON_ACT_TRESH && findExchangeCard(trumpSuit, hand) < (int) hand.size()) return M_EXCHANGE;

    int targetMarriageSuit = -1;
    std::vector<bool> marriageSuits = findMarriageSuits(hand);
    for (int i = 0; i < NUM_SUITS; ++i)
    {
        if (marriageSuits[i]) targetMarriageSuit = i;
    }
    if (marriageSuits[trumpSuit]) targetMarriageSuit = trumpSuit;
    if (trickNumber > 0 && targetMarriageSuit != -1)
    {
        for (int i = 0; i < (int) hand.size(); ++i)
        {
            if (hand[i].suit == targetMarriageSuit && hand[i].rank == MARRIAGE_RANKS.front()) return i;
        }
    }

    int move = -1;
    bool targetBetter = closed || talonSize == 0;
    for (int i = 0; i < (int) hand.size(); ++i)
    {
        if (move == -1 || isBetter(trumpSuit, hand[i], hand[move]) == targetBetter) move = i;
    }

    return move;
}

int PlayerSimple::getResponse(const std::vector<int>& valid)
{
    std::vector<int> priorities(hand.size(), 0);
    std::vector<bool> marriageSuits = findMarriageSuits(hand);

    for (int i : valid)
    {
        if (hand[i].suit == trumpSuit) priorities[i] -= 5 + hand[i].rank;
        if (marriageSuits[hand[i].suit] && isMarriageCard(hand[i])) priorities[i] -= hand[i].suit != trumpSuit ? REG_MARRIAGE_VALUE : TRUMP_MARRIAGE_VALUE;
        if (!leadWinsTrick(trumpSuit, leadCard, hand[i])) priorities[i] += CARD_VALUES[leadCard.rank] + (hand[i].suit != trumpSuit) * CARD_VALUES[hand[i].suit];
        else priorities[i] -= CARD_VALUES[leadCard.rank] + CARD_VALUES[hand[i].suit];
    }

    int response = -1;
    for (int i : valid)
    {
        if (response == -1 || priorities[i] > priorities[response]) response = i;
    }

    ++trickNumber;
    return response;
}
