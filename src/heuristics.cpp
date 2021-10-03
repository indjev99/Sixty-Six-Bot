
#include "game_state.h"
#include "config.h"
#include "util.h"
#include <algorithm>

static bool operator==(const CardAnnotated& left, const CardAnnotated& right)
{
    return left.card.suit == right.card.suit && left.card.rank == right.card.rank && left.canMarry == right.canMarry && left.canExchange == right.canExchange;
}

static bool operator<=(const CardAnnotated& left, const CardAnnotated& right)
{
    return left.card.suit == right.card.suit && left.card.rank <= right.card.rank && left.canMarry <= right.canMarry && left.canExchange <= right.canExchange;
}

// TODO: try canMarry only if have marriage
// TODO: try keeping track of played cards (and give them on construction) -- important for taking responses but also for canMarry

static const int KEEP_TRUMP_VALUE = CARD_VALUES[NUM_RANKS - 1];

std::vector<int> GameState::recommendedActions(bool experimental)
{
    validActions();

    if (move.type == M_NONE)
    {
        if (std::find(tmpValid.begin(), tmpValid.end(), M_EXCHANGE) != tmpValid.end()) tmpRecommended = {M_EXCHANGE};
        else std::swap(tmpValid, tmpRecommended);
        return tmpRecommended;
    }

    const std::vector<Card>& hand = respState.hand;

    tmpLowest.clear();
    for (Card card : hand)
    {
        if (!leadWinsTrick(trumpSuit, move.card, card)) continue;

        bool canMarry = isMarriageRank(card.rank) && (respState.isMarriageSuit[card.suit] || (!closed && talon.size() > 0));
        bool canExchange = card.rank == EXCHANGE_RANK && card.suit == trumpSuit && !closed && talon.size() >= TALON_ACT_TRESH + 4;
        CardAnnotated curr = {card, canMarry, canExchange};

        bool relevant = true;
        for (auto it = tmpLowest.begin(); it != tmpLowest.end(); )
        {
            if (*it <= curr)
            {
                relevant = false;
                break;
            }
            if (curr <= *it) it = tmpLowest.erase(it);
            else ++it;
        }
        if (relevant) tmpLowest.push_back(curr);
    }

    tmpRecommended.clear();
    for (int action : tmpValid)
    {
        Card card = hand[action];
        if (!leadWinsTrick(trumpSuit, move.card, card))
        {
            bool nextCanMarry = isMarriageRank(card.rank + 1) && !isMarriageRank(card.rank) && (respState.isMarriageSuit[card.suit] || (!closed && talon.size() > 0));
            bool nextCanExchange = card.rank + 1 == EXCHANGE_RANK && card.suit == trumpSuit && !closed && talon.size() >= TALON_ACT_TRESH + 2;
            bool haveNext = std::find(hand.begin(), hand.end(), Card(card.rank + 1, card.suit)) != hand.end();
            if (nextCanMarry || nextCanExchange || !haveNext) tmpRecommended.push_back(action);
        }
        else
        {
            bool canMarry = isMarriageRank(card.rank) && (respState.isMarriageSuit[card.suit] || (!closed && talon.size() > 0));
            bool canExchange = card.rank == EXCHANGE_RANK && card.suit == trumpSuit && !closed && talon.size() >= TALON_ACT_TRESH + 4;
            CardAnnotated curr = {card, canMarry, canExchange};
            if (std::find(tmpLowest.begin(), tmpLowest.end(), curr) != tmpLowest.end()) tmpRecommended.push_back(action);
        }
    }

    tmpPriorities.resize(hand.size());
    for (int i : tmpRecommended)
    {
        tmpPriorities[i] = 0;
        if (hand[i].suit == trumpSuit) tmpPriorities[i] -= KEEP_TRUMP_VALUE + hand[i].rank;
        if (respState.isMarriageSuit[hand[i].suit] && isMarriageRank(hand[i].rank)) tmpPriorities[i] -= hand[i].suit != trumpSuit ? REG_MARRIAGE_VALUE : TRUMP_MARRIAGE_VALUE;
        if (!leadWinsTrick(trumpSuit, move.card, hand[i])) tmpPriorities[i] += CARD_VALUES[move.card.rank] + (hand[i].suit != trumpSuit) * CARD_VALUES[hand[i].rank];
        else tmpPriorities[i] -= CARD_VALUES[move.card.rank] + CARD_VALUES[hand[i].rank];
    }

    std::sort(tmpRecommended.begin(), tmpRecommended.end(), 
        [this](int left, int right) { return tmpPriorities[left] > tmpPriorities[right]; }
    );

    return tmpRecommended;
}

double GameState::actionHeuristic(int idx)
{
    if (tmpRecommended.size() == 1) return 0;

    if (move.type == M_NONE)
    {
        // TODO: add heurisitcs about 20, 40, closing
        return 0;
    }

    int pos = std::find(tmpRecommended.begin(), tmpRecommended.end(), idx) - tmpRecommended.begin();
    return  -0.5 * pos / (tmpRecommended.size() - 1);
}
