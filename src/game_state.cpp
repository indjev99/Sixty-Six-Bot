
#include "game_state.h"
#include "config.h"
#include "util.h"
#include <algorithm>
#include <numeric>

PlayerGameState::PlayerGameState(int mult, Player* player):
    mult(mult),
    player(player),
    score(0),
    hasTakenTricks(false),
    hasClosed(false) {}

PlayerGameState::PlayerGameState(int mult, int score, bool hasTakenTrick, bool hasClosed, const std::vector<Card>& hand):
    mult(mult),
    player(nullptr),
    score(score),
    hasTakenTricks(hasTakenTrick),
    hasClosed(hasClosed),
    hand(hand) {}

GameState::GameState(Player* leadPlayer, Player* respPlayer):
    trickNumber(0),
    closed(0),
    move(),
    leadState(1, leadPlayer),
    respState(-1, respPlayer),
    talon(),
    noActionPlayer(0)
{
    trumpSuit = talon.lastCard().suit;
    leadState.hand = talon.dealHand();
    respState.hand = talon.dealHand();

    if (leadState.player && respState.player)
    {
        leadState.player->startGame();
        respState.player->startGame();
    }
}

GameState::GameState(int trumpSuit, int trickNumber, bool closed, Move move, const PlayerGameState& leadState,
                     const PlayerGameState& respState, const std::vector<Card>& talon):
    trumpSuit(NUM_SUITS),
    trickNumber(0),
    closed(0),
    move(),
    leadState(leadState),
    respState(respState),
    talon(talon),
    noActionPlayer(0)
{
    if (leadState.player && respState.player)
    {
        leadState.player->startGame();
        respState.player->startGame();
    }
}

void GameState::setPlayers(Player* leadPlayer, Player* respPlayer)
{
    if (leadState.mult < 0) std::swap(leadPlayer, respPlayer);

    leadState.player = leadPlayer;
    respState.player = respPlayer;

    if (leadState.player && respState.player)
    {
        leadState.player->startGame();
        respState.player->startGame();
    }
}

int GameState::currentPlayer()
{
    if (move.type == M_NONE) return leadState.mult;
    else return respState.mult;
}

std::vector<int> GameState::validActions()
{
    if (move.type == M_NONE)
    {
        if (leadState.player && respState.player)
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
            std::vector<int> suitedRaises;
            std::vector<int> suited;
            std::vector<int> trumps;

            for (int i = 0; i < (int) respState.hand.size(); ++i)
            {
                Card card = respState.hand[i];
                if (card.suit == trumpSuit) trumps.push_back(i);
                if (card.suit == move.card.suit) suited.push_back(i);
                if (card.suit == move.card.suit && card.rank > move.card.rank) suitedRaises.push_back(i);
            }

            if (!suitedRaises.empty()) return suitedRaises;
            else if (!suited.empty()) return suited;
            else if (!trumps.empty()) return trumps;
        }

        std::vector<int> valid(respState.hand.size());
        std::iota(valid.begin(), valid.end(), 0);
        return valid;
    }
}

void GameState::applyAction(int idx)
{
    if (move.type == M_NONE)
    {
        move.score = 0;
        if (idx >= 0)
        {
            move.type = M_PLAY;
            move.card = leadState.hand[idx];
            std::vector<bool> isMarriageSuit = findMarriageSuits(leadState.hand);
            if (trickNumber > 0 && isMarriageSuit[move.card.suit] && isMarriageCard(move.card.rank))
            {
                move.score = move.card.suit == trumpSuit ? TRUMP_MARRIAGE_VALUE : REG_MARRIAGE_VALUE;
            }
        }
        else move.type = idx;

        if (leadState.player && respState.player) respState.player->giveMove(move);

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

        if (leadState.player && respState.player) leadState.player->giveResponse(response);

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

    if (leadState.player && respState.player)
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

int GameState::actionCode(int idx)
{
    if (idx < 0) return idx;
    if (move.type == M_NONE) return leadState.hand[idx].code();
    else return respState.hand[idx].code();
}
