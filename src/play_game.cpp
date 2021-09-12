#include "play_game.h"
#include "talon.h"
#include "util.h"
#include "config.h"
#include <algorithm>
#include <numeric>

const int MAX_ATTEMPTS = 32;

struct PlayerGameState
{
    int mult;
    Player* player;

    std::vector<Card> hand;
    int score = 0;
    bool hasTakenTricks = false;
    bool hasClosed = false;
};

int playGame(Player* leadPlayer, Player* respPlayer)
{
    Talon talon;
    bool closed = false;
    int trumpSuite = talon.lastCard().suite;

    PlayerGameState leadState, respState;

    leadState.mult = 1;
    leadState.player = leadPlayer;
    leadState.hand = talon.dealHand();

    respState.mult = -1;
    respState.player = respPlayer;
    respState.hand = talon.dealHand();

    leadState.player->reset();
    respState.player->reset();

    while ((leadState.score < WIN_TRESH || !leadState.hasTakenTricks) && !leadState.hand.empty())
    {
        leadState.player->giveState(closed, talon.size(), talon.lastCard(), leadState.score, respState.score);
        respState.player->giveState(closed, talon.size(), talon.lastCard(), respState.score, leadState.score);

        std::sort(leadState.hand.begin(), leadState.hand.end());
        leadState.player->giveHand(leadState.hand);

        std::sort(respState.hand.begin(), respState.hand.end());
        respState.player->giveHand(respState.hand);

        bool canDoTalonAct = !closed && leadState.hasTakenTricks && talon.size() >= TALON_ACT_TRESH;
        int exchangeIdx = findExchangeCard(trumpSuite, leadState.hand);
        std::vector<bool> marriageSuites = findMarriageSuits(leadState.hand);

        int moveIdx;
        int attempts = MAX_ATTEMPTS;
        do
        {
            if (!--attempts) return -NOMOVE_POINTS * leadState.mult;
            moveIdx = leadState.player->getMove();
        }
        while (moveIdx >= (int) leadState.hand.size() || moveIdx < M_EXCHANGE ||
               (moveIdx == M_CLOSE && !canDoTalonAct) ||
               (moveIdx == M_EXCHANGE && (!canDoTalonAct || exchangeIdx == (int) leadState.hand.size())));

        Move move;
        if (moveIdx >= 0)
        {
            move.type = M_PLAY;
            move.card = leadState.hand[moveIdx];
            if (leadState.hasTakenTricks && marriageSuites[move.card.suite] && std::find(MARRIAGE_RANKS.begin(), MARRIAGE_RANKS.end(), move.card.rank) != MARRIAGE_RANKS.end())
            {
                move.score = move.card.suite == trumpSuite ? TRUMP_MARRIAGE_VALUE : REG_MARRIAGE_VALUE;
            }
        }
        else move.type = moveIdx;
        respState.player->giveMove(move);

        if (move.type == M_PLAY)
        {
            leadState.hand.erase(leadState.hand.begin() + moveIdx);
            leadState.score += move.score;
            if (leadState.score >= WIN_TRESH && leadState.hasTakenTricks) break;
        }
        else if (move.type == M_CLOSE)
        {
            closed = true;
            leadState.hasClosed = true;
            continue;
        }
        else if (move.type == M_EXCHANGE)
        {
            Card temp = leadState.hand[exchangeIdx];
            leadState.hand[exchangeIdx] = talon.lastCard();
            talon.setLastCard(temp);
            continue;
        }

        std::vector<int> validResps;

        if (closed || talon.size() == 0) validResps = findValidResponses(trumpSuite, move.card, respState.hand);
        else
        {
            validResps.resize(respState.hand.size());
            std::iota(validResps.begin(), validResps.end(), 0);
        }

        attempts = MAX_ATTEMPTS;
        int responseIdx;
        do
        {
            if (!--attempts) return -NOMOVE_POINTS * respState.mult;
            responseIdx = respState.player->getResponse(validResps);
        }
        while (std::find(validResps.begin(), validResps.end(), responseIdx) == validResps.end());

        Card response = respState.hand[responseIdx];
        leadState.player->giveResponse(response);
        respState.hand.erase(respState.hand.begin() + responseIdx);

        if (!leadWinsTrick(trumpSuite, move.card, response)) std::swap(leadState, respState);

        leadState.score += CARD_VALUES[move.card.rank] + CARD_VALUES[response.rank];
        leadState.hasTakenTricks = true;

        if (!closed && talon.size() > 0)
        {
            leadState.hand.push_back(talon.dealCard());
            respState.hand.push_back(talon.dealCard());
        }
    }

    if (!closed && leadState.hand.empty()) leadState.score += LAST_TRICK_VALUE;

    leadState.player->giveState(closed, talon.size(), talon.lastCard(), leadState.score, respState.score);
    respState.player->giveState(closed, talon.size(), talon.lastCard(), respState.score, leadState.score);

    if (closed && !leadState.hasClosed) std::swap(leadState, respState);

    int points = 0;
    if (leadState.score >= WIN_TRESH)
    {
        points = WIN_POINTS;
        if (respState.score < BIGWIN_OPP_TRESH) points = BIGWIN_POINTS;
        if (!respState.hasTakenTricks) points = VALATWIN_POINTS;
    }
    else if (leadState.hasClosed) points = -CLOSEFAIL_POINTS;

    leadState.player->giveGameResult(points);
    respState.player->giveGameResult(-points);

    return points * leadState.mult;
}
