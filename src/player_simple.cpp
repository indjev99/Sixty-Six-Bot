#include "player_simple.h"
#include "config.h"
#include "util.h"
#include "rng.h"
#include <algorithm>
#include <numeric>

static const int CLOSE_TAKE_VALUE = (std::accumulate(CARD_VALUES, CARD_VALUES + NUM_RANKS, 0) + NUM_RANKS / 2) / NUM_RANKS;
static const int KEEP_TRUMP_VALUE = CARD_VALUES[NUM_RANKS - 1];

void PlayerSimple::startSet() {}

void PlayerSimple::startGame()
{
    trickNumber = 0;
}

void PlayerSimple::giveState(bool closed, int talonSize, Card trumpCard, int selfScore, int oppScore)
{
    if (trickNumber == 0) trumpSuit = trumpCard.suit;
    this->closed = closed;
    this->talonSize = talonSize;
    this->selfScore = selfScore;
}

void PlayerSimple::giveHand(const std::vector<Card>& hand)
{
    this->hand = hand;
}

void PlayerSimple::giveMove(Move move, bool self)
{
    leadCard = move.card;
}

void PlayerSimple::giveResponse(Card card, bool self)
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

int PlayerSimple::getMove(const std::vector<int>& valid)
{
    if (std::find(valid.begin(), valid.end(), M_EXCHANGE) != valid.end()) return M_EXCHANGE;

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

    if (std::find(valid.begin(), valid.end(), M_CLOSE) != valid.end())
    {
        int prevSuit = NUM_SUITS;
        int closingScore = selfScore;
        bool canTake = false;
        int numTrumpsTaken = 0;
        for (int i = hand.size(); i >= 0; --i)
        {
            if (hand[i].suit != prevSuit) canTake = hand[i].rank == R_ACE;
            else canTake = canTake && (hand[i].rank == hand[i + 1].rank - 1);
    
            if (canTake) closingScore += CARD_VALUES[hand[i].rank] + CLOSE_TAKE_VALUE;
            if (canTake && hand[i].suit == trumpSuit) ++numTrumpsTaken;

            prevSuit = hand[i].suit;
        }

        if (numTrumpsTaken >= (NUM_RANKS - 1) / 2 && closingScore >= WIN_TRESH) return M_CLOSE;
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
        if (hand[i].suit == trumpSuit) priorities[i] -= KEEP_TRUMP_VALUE + hand[i].rank;
        if (marriageSuits[hand[i].suit] && isMarriageCard(hand[i].rank)) priorities[i] -= hand[i].suit != trumpSuit ? REG_MARRIAGE_VALUE : TRUMP_MARRIAGE_VALUE;
        if (!leadWinsTrick(trumpSuit, leadCard, hand[i])) priorities[i] += CARD_VALUES[leadCard.rank] + (hand[i].suit != trumpSuit) * CARD_VALUES[hand[i].rank];
        else priorities[i] -= CARD_VALUES[leadCard.rank] + CARD_VALUES[hand[i].rank];
    }

    int response = -1;
    for (int i : valid)
    {
        if (response == -1 || priorities[i] > priorities[response]) response = i;
    }

    return response;
}
