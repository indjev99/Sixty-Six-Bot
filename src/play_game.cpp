#include "play_game.h"
#include "talon.h"
#include "config.h"
#include <algorithm>

struct PlayerGameState
{
    int id;
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
    int trump = talon.lastCard().suite;

    PlayerGameState leadState, respState;

    leadState.id = 1;
    leadState.player = leadPlayer;
    leadState.hand = talon.dealHand();
    leadState.player->giveTrump(talon.lastCard());

    respState.id = 2;
    respState.player = respPlayer;
    respState.hand = talon.dealHand();
    respState.player->giveTrump(talon.lastCard());

    while ((leadState.score < WIN_TRESH || !leadState.hasTakenTricks) && !leadState.hand.empty())
    {
        std::sort(leadState.hand.begin(), leadState.hand.end());
        leadState.player->giveHand(leadState.hand);

        std::sort(respState.hand.begin(), respState.hand.end());
        respState.player->giveHand(respState.hand);

        bool canDoTalonAct = !closed && leadState.hasTakenTricks && talon.size() >= TALON_ACT_TRESH;
        int exchangeIdx = -1;
        std::vector<int> marriageCounts(NUM_SUITES, 0);

        for (int i = 0; i < (int) leadState.hand.size(); ++i)
        {
            Card card = leadState.hand[i];
            if (card.suite == trump && card.rank == EXCHANGE_RANK) exchangeIdx = i;
            if (std::find(MARRIAGE_RANKS.begin(), MARRIAGE_RANKS.end(), card.rank) != MARRIAGE_RANKS.end()) ++marriageCounts[card.suite];
        }

        // TODO: limit number of tries
        int moveIdx;
        do
        {
            moveIdx = leadState.player->getMove();
        }
        while (moveIdx >= (int) leadState.hand.size() || moveIdx < M_EXCHANGE ||
               (moveIdx == M_CLOSE && !canDoTalonAct) ||
               (moveIdx == M_EXCHANGE && (!canDoTalonAct || exchangeIdx == -1)));

        Move move;
        if (moveIdx >= 0)
        {
            move.type = M_PLAY;
            move.card = leadState.hand[moveIdx];
            if (marriageCounts[move.card.suite] == (int) MARRIAGE_RANKS.size())
            {
                move.score = move.card.suite == trump ? TRUMP_MARRIAGE_VALUE : REG_MARRIAGE_VALUE;
            }
        }
        else move.type = moveIdx;

        leadState.player->giveMove(move, true);
        respState.player->giveMove(move, false);

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

        // TODO: require correct responses when closed or talon is empty
        // TODO: limit number of tries
        int responseIdx;
        do
        {
            responseIdx = respState.player->getResponse();
        }
        while (responseIdx >= (int) respState.hand.size() || responseIdx < 0);

        Card response = respState.hand[moveIdx];

        leadState.player->giveResponse(response, false);
        respState.player->giveResponse(response, true);

        respState.hand.erase(respState.hand.begin() + responseIdx);

        bool leadWinsHand = true;
        if (move.card.suite == response.suite && move.card.rank < response.rank) leadWinsHand = false;
        if (move.card.suite != trump && response.suite == trump) leadWinsHand = false;

        if (!leadWinsHand) std::swap(leadState, respState);

        leadState.score += CARD_VALUES[move.card.rank] + CARD_VALUES[response.rank];
        leadState.hasTakenTricks = true;

        if (!closed && talon.size() > 0)
        {
            leadState.hand.push_back(talon.dealCard());
            respState.hand.push_back(talon.dealCard());
        }
    }

    if (!closed && leadState.hand.empty()) leadState.score += LAST_TRICK_VALUE;

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
    leadState.player->giveGameResult(-points);

    if (leadState.id == 2)
    {
        points = -points;
        std::swap(leadState, respState);
    }

    return points;
}
