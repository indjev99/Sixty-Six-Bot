
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
// TODO: prune taking responses
// TODO: keep track of played cards (and give them on construction) -- important for taking responses but also for canMarry

std::vector<int> GameState::recommendedActions(bool experimental)
{
    validActions();

    if (move.type == M_NONE)
    {
        if (std::find(tmpValid.begin(), tmpValid.end(), M_EXCHANGE) != tmpValid.end()) tmpRecommended = {M_EXCHANGE};
        else std::swap(tmpValid, tmpRecommended);
        return tmpRecommended;
    }

    tmpLowest.clear();
    for (Card card : respState.hand)
    {
        if (!leadWinsTrick(trumpSuit, move.card, card)) continue;

        bool canMarry = isMarriageRank(card.rank) && (respState.isMarriageSuit[card.suit] || (!closed && talon.size() > 0));
        bool canExchange = card.suit == Card(EXCHANGE_RANK, trumpSuit) && !closed && talon.size() >= TALON_ACT_TRESH + 4;
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
        Card card = respState.hand[action];
        if (!leadWinsTrick(trumpSuit, move.card, card))
        {
            tmpRecommended.push_back(action);
            continue;
        }

        bool canMarry = isMarriageRank(card.rank) && (respState.isMarriageSuit[card.suit] || (!closed && talon.size() > 0));
        bool canExchange = card.suit == Card(EXCHANGE_RANK, trumpSuit) && !closed && talon.size() >= TALON_ACT_TRESH + 4;
        CardAnnotated curr = {card, canMarry, canExchange};

        if (std::find(tmpLowest.begin(), tmpLowest.end(), curr) != tmpLowest.end()) tmpRecommended.push_back(action);
    }

    return tmpRecommended;
}
