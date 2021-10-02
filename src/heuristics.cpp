
#include "game_state.h"
#include "config.h"
#include "util.h"
#include <algorithm>

struct CardResponse
{
    int rank;
    bool canMarry;
    bool canExchange;
};

static bool operator==(const CardResponse& left, const CardResponse& right)
{
    return left.rank == right.rank && left.canMarry == right.canMarry && left.canExchange == right.canExchange;
}

static bool operator<(const CardResponse& left, const CardResponse& right)
{
    return left.rank <= right.rank && left.canMarry <= right.canMarry && left.canExchange <= right.canExchange;
}

std::vector<int> GameState::recommendedActions()
{
    std::vector<int> valid = validActions();

    if (move.type == M_NONE)
    {
        if (std::find(valid.begin(), valid.end(), M_EXCHANGE) != valid.end()) return {M_EXCHANGE};
        else return valid;
    }
    else
    {
        std::vector<std::vector<CardResponse>> lowest(NUM_SUITS);
        for (Card card : respState.hand)
        {
            if (!leadWinsTrick(trumpSuit, move.card, card)) continue;

            bool canMarry = isMarriageRank(card.rank) && (respState.isMarriageSuit[card.suit] || (!closed && talon.size() > 0));
            bool canExchange = card.suit == Card(EXCHANGE_RANK, trumpSuit) && !closed && talon.size() >= TALON_ACT_TRESH + 4;
            CardResponse curr = {card.rank, canMarry, canExchange};

            bool relevant = true;
            std::vector<CardResponse>& suitLowest = lowest[card.suit];
            for (auto it = suitLowest.begin(); it != suitLowest.end(); )
            {
                if (*it < curr)
                {
                    relevant = false;
                    break;
                }
                if (curr < *it) it = suitLowest.erase(it);
                else ++it;
            }
            if (relevant) suitLowest.push_back(curr);
        }

        std::vector<int> actions;
        for (int action : valid)
        {
            Card card = respState.hand[action];
            if (!leadWinsTrick(trumpSuit, move.card, card))
            {
                actions.push_back(action);
                continue;
            }

            bool canMarry = isMarriageRank(card.rank) && (respState.isMarriageSuit[card.suit] || (!closed && talon.size() > 0));
            bool canExchange = card.suit == Card(EXCHANGE_RANK, trumpSuit) && !closed && talon.size() >= TALON_ACT_TRESH + 4;
            CardResponse curr = {card.rank, canMarry, canExchange};

            std::vector<CardResponse>& suitLowest = lowest[card.suit];
            if (std::find(suitLowest.begin(), suitLowest.end(), curr) != suitLowest.end()) actions.push_back(action);
        }

        return actions;
    }
}
