
#include "game_state.h"
#include "config.h"
#include "util.h"
#include <algorithm>
#include <numeric>

GameState::GameState(Player* leadPlayer, Player* respPlayer)
{
    closed = false;
    trickNumber = 0;
    trumpSuit = talon.lastCard().suit;

    move.type = M_NONE;

    leadState.mult = 1;
    leadState.player = leadPlayer;
    leadState.hand = talon.dealHand();
    leadState.score = 0;
    leadState.hasTakenTricks = false;
    leadState.hasClosed = false;

    respState.mult = -1;
    respState.player = respPlayer;
    respState.hand = talon.dealHand();
    respState.score = 0;
    respState.hasTakenTricks = false;
    respState.hasClosed = false;

    noActionPlayer = 0;
    giveInfo = leadState.player && respState.player;

    if (giveInfo)
    {
        leadState.player->startGame();
        respState.player->startGame();
    }
}

std::vector<int> GameState::validActions()
{
    if (move.type == M_NONE)
    {
        if (giveInfo)
        {
            leadState.player->giveState(closed, talon.size(), talon.lastCard(), leadState.score, respState.score);
            respState.player->giveState(closed, talon.size(), talon.lastCard(), respState.score, leadState.score);

            std::sort(leadState.hand.begin(), leadState.hand.end());
            std::sort(respState.hand.begin(), respState.hand.end());

            leadState.player->giveHand(leadState.hand);
            respState.player->giveHand(respState.hand);
        }

        std::vector<int> valid;
        valid.resize(leadState.hand.size());
        std::iota(valid.begin(), valid.end(), 0);

        bool canDoTalonAct = !closed && trickNumber > 0 && talon.size() >= TALON_ACT_TRESH;
        if (canDoTalonAct)
        {
            valid.push_back(M_CLOSE);
            int exchangeIdx = findExchangeCard(trumpSuit, leadState.hand);
            if (exchangeIdx < (int) leadState.hand.size()) valid.push_back(M_EXCHANGE);
        }

        return valid;
    }
    else
    {
        if (closed || talon.size() == 0)
        {
            return findValidResponses(trumpSuit, move.card, respState.hand);
        }
        else
        {
            std::vector<int> valid;
            valid.resize(respState.hand.size());
            std::iota(valid.begin(), valid.end(), 0);
            return valid;
        }
    }
}

void GameState::applyAction(int idx)
{
    Move retVal;
    if (move.type == M_NONE)
    {
        move.score = 0;
        if (idx >= 0)
        {
            move.type = M_PLAY;
            move.card = leadState.hand[idx];
            std::vector<bool> isMarriageSuit = findMarriageSuits(leadState.hand);
            if (trickNumber > 0 && isMarriageSuit[move.card.suit] && isMarriageCard(move.card))
            {
                move.score = move.card.suit == trumpSuit ? TRUMP_MARRIAGE_VALUE : REG_MARRIAGE_VALUE;
            }
        }
        else move.type = idx;

        if (giveInfo) respState.player->giveMove(move);

        if (move.type == M_PLAY)
        {
            leadState.hand.erase(leadState.hand.begin() + idx);
            leadState.score += move.score;
        }
        else if (move.type == M_CLOSE)
        {
            closed = true;
            leadState.hasClosed = true;
            move.type = M_NONE;
        }
        else if (move.type == M_EXCHANGE)
        {
            int exchangeIdx = findExchangeCard(trumpSuit, leadState.hand);
            Card temp = leadState.hand[exchangeIdx];
            leadState.hand[exchangeIdx] = talon.lastCard();
            talon.setLastCard(temp);
            move.type = M_NONE;
        }
    }
    else
    {
        Card response = respState.hand[idx];
        respState.hand.erase(respState.hand.begin() + idx);

        if (giveInfo) leadState.player->giveResponse(response);

        if (!leadWinsTrick(trumpSuit, move.card, response)) std::swap(leadState, respState);

        leadState.score += CARD_VALUES[move.card.rank] + CARD_VALUES[response.rank];
        leadState.hasTakenTricks = true;

        if (!closed && talon.size() > 0)
        {
            leadState.hand.push_back(talon.dealCard());
            respState.hand.push_back(talon.dealCard());
        }

        if (!closed && leadState.hand.empty()) leadState.score += LAST_TRICK_VALUE;

        move.type = M_NONE;
        ++trickNumber;
    }
}

bool GameState::isTerminal()
{
    return leadState.score >= WIN_TRESH || leadState.hand.empty() || noActionPlayer;
}

int GameState::result()
{
    if (noActionPlayer) return noActionPlayer * -NOACTION_POINTS;

    if (giveInfo)
    {
        leadState.player->giveState(closed, talon.size(), talon.lastCard(), leadState.score, respState.score);
        respState.player->giveState(closed, talon.size(), talon.lastCard(), respState.score, leadState.score);
    }

    if (closed && !leadState.hasClosed) std::swap(leadState, respState);

    int points = 0;
    if (leadState.score >= WIN_TRESH)
    {
        points = WIN_POINTS;
        if (respState.score < BIGWIN_OPP_TRESH) points = BIGWIN_POINTS;
        if (!respState.hasTakenTricks) points = VALATWIN_POINTS;
    }
    else if (leadState.hasClosed) points = -CLOSEFAIL_POINTS;

    return points * leadState.mult;
}

void GameState::applyPlayerAction(int attempts)
{
    int idx;
    std::vector<int> valid = validActions();
    do
    {
        if (move.type == M_NONE) idx = leadState.player->getMove(valid);
        else idx = respState.player->getResponse(valid);
    }
    while (attempts-- && std::find(valid.begin(), valid.end(), idx) == valid.end());

    if (attempts < 0)
    {
        noActionPlayer = (move.type == M_NONE ? leadState : respState).mult;
        return;
    }

    applyAction(idx);
}