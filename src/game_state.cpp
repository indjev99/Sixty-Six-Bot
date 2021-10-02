
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
    hasClosed(false),
    isMarriageSuit(NUM_SUITS),
    marriageCardCounts(NUM_SUITS) {}

void PlayerGameState::setHand(const std::vector<Card>& hand)
{
    this->hand = hand;

    std::fill(marriageCardCounts.begin(), marriageCardCounts.end(), 0);
    for (int i = 0; i < (int) hand.size(); ++i)
    {
        if (isMarriageRank(hand[i].rank)) ++marriageCardCounts[hand[i].suit];
    }

    for (int i = 0; i < NUM_SUITS; ++i)
    {
        isMarriageSuit[i] = marriageCardCounts[i] == (int) MARRIAGE_RANKS.size();
    }
}

void PlayerGameState::addCard(Card card)
{
    if (isMarriageRank(card.rank)) ++marriageCardCounts[card.suit];
    isMarriageSuit[card.suit] = marriageCardCounts[card.suit] == (int) MARRIAGE_RANKS.size();
    hand.push_back(card);
}

void PlayerGameState::removeCard(int idx)
{
    Card card = hand[idx];
    if (isMarriageRank(card.rank)) --marriageCardCounts[card.suit];
    isMarriageSuit[card.suit] = marriageCardCounts[card.suit] == (int) MARRIAGE_RANKS.size();
    hand.erase(hand.begin() + idx);
}

namespace std
{
    void swap(PlayerGameState& left, PlayerGameState& right)
    {
        std::swap(left.mult, right.mult);
        std::swap(left.player, right.player);
        std::swap(left.score, right.score);
        std::swap(left.hasTakenTricks, right.hasTakenTricks);
        std::swap(left.hasClosed, right.hasClosed);
        std::swap(left.hand, right.hand);
        std::swap(left.isMarriageSuit, right.isMarriageSuit);
        std::swap(left.marriageCardCounts, right.marriageCardCounts);
    }
}

GameState::GameState(Player* leadPlayer, Player* respPlayer):
    trickNumber(0),
    closed(0),
    move(),
    leadState(1, leadPlayer),
    respState(-1, respPlayer),
    talon()
{
    trumpSuit = talon.lastCard().suit;
    leadState.setHand(talon.dealHand());
    respState.setHand(talon.dealHand());

    if (leadState.player && respState.player)
    {
        leadState.player->startGame();
        respState.player->startGame();
    }
}

GameState::GameState(int trumpSuit, int trickNumber, bool closed, Move move, const PlayerGameState& leadState,
                     const PlayerGameState& respState, const std::vector<Card>& talon):
    trumpSuit(trumpSuit),
    trickNumber(trickNumber),
    closed(closed),
    move(move),
    leadState(leadState),
    respState(respState),
    talon(talon) {}

void GameState::reserveMem()
{
    tmpValid.reserve(HAND_SIZE + 2);
    tmpSuitedRaises.reserve(HAND_SIZE);
    tmpSuited.reserve(HAND_SIZE);
    tmpTrumps.reserve(HAND_SIZE);
    tmpRecommended.reserve(HAND_SIZE + 2);
    tmpLowest.reserve(HAND_SIZE);
}

void GameState::setPlayers(Player* leadPlayer, Player* respPlayer)
{
    if (leadState.mult == -1) std::swap(leadPlayer, respPlayer);

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
            Card lastCard;
            if (talon.size() > 0) lastCard = talon.lastCard();
            leadState.player->giveState(closed, talon.size(), lastCard, leadState.score, respState.score);
            respState.player->giveState(closed, talon.size(), lastCard, respState.score, leadState.score);
            leadState.player->giveHand(leadState.hand);
            respState.player->giveHand(respState.hand);
        }

        tmpValid.resize(leadState.hand.size());
        std::iota(tmpValid.begin(), tmpValid.end(), 0);

        bool canDoTalonAct = !closed && trickNumber > 0 && talon.size() >= TALON_ACT_TRESH;
        if (canDoTalonAct)
        {
            tmpValid.push_back(M_CLOSE);
            int exchangeIdx = findExchangeCard(trumpSuit, leadState.hand);
            if (exchangeIdx < (int) leadState.hand.size()) tmpValid.push_back(M_EXCHANGE);
        }

        return tmpValid;
    }
    else
    {
        if (closed || talon.size() == 0)
        {
            tmpSuitedRaises.clear();
            tmpSuited.clear();
            tmpTrumps.clear();

            for (int i = 0; i < (int) respState.hand.size(); ++i)
            {
                Card card = respState.hand[i];
                if (card.suit == move.card.suit && card.rank > move.card.rank) tmpSuitedRaises.push_back(i);
                else if (card.suit == move.card.suit) tmpSuited.push_back(i);
                else if (card.suit == trumpSuit) tmpTrumps.push_back(i);
            }

            bool anyCard = false;

            if (!tmpSuitedRaises.empty()) std::swap(tmpSuitedRaises, tmpValid);
            else if (!tmpSuited.empty()) std::swap(tmpSuited, tmpValid);
            else if (!tmpTrumps.empty()) std::swap(tmpTrumps, tmpValid);
            else anyCard = true;

            if (!anyCard) return tmpValid;
        }

        tmpValid.resize(respState.hand.size());
        std::iota(tmpValid.begin(), tmpValid.end(), 0);
        return tmpValid;
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
            if (trickNumber > 0 && leadState.isMarriageSuit[move.card.suit] && isMarriageRank(move.card.rank))
            {
                move.score = move.card.suit == trumpSuit ? TRUMP_MARRIAGE_VALUE : REG_MARRIAGE_VALUE;
            }
        }
        else move.type = idx;

        if (leadState.player && respState.player)
        {
            leadState.player->giveMove(move, true);
            respState.player->giveMove(move, false);
        }

        if (move.type == M_PLAY)
        {
            leadState.removeCard(idx);
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
            leadState.removeCard(exchangeIdx);
            leadState.addCard(talon.lastCard());
            talon.setLastCard({EXCHANGE_RANK, trumpSuit});
            move.type = M_NONE;
        }
    }
    else
    {
        Card response = respState.hand[idx];
        respState.removeCard(idx);

        if (leadState.player && respState.player)
        {
            leadState.player->giveResponse(response, false);
            respState.player->giveResponse(response, true);
        }

        if (!leadWinsTrick(trumpSuit, move.card, response)) std::swap(leadState, respState);

        leadState.score += CARD_VALUES[move.card.rank] + CARD_VALUES[response.rank];
        leadState.hasTakenTricks = true;

        if (!closed && talon.size() > 0)
        {
            if (talon.size() > 2 && leadState.mult == -1)
            {
                respState.addCard(talon.dealCard());
                leadState.addCard(talon.dealCard());
            }
            else
            {
                leadState.addCard(talon.dealCard());
                respState.addCard(talon.dealCard());
            }
        }

        if (!closed && leadState.hand.empty()) leadState.score += LAST_TRICK_VALUE;

        move.type = M_NONE;
        ++trickNumber;
    }
}

bool GameState::isTerminal()
{
    return leadState.score >= WIN_TRESH || respState.hand.empty();
}

int GameState::result()
{
    if (leadState.player && respState.player)
    {
        Card lastCard;
        if (talon.size() > 0) lastCard = talon.lastCard();
        leadState.player->giveState(closed, talon.size(), lastCard, leadState.score, respState.score);
        respState.player->giveState(closed, talon.size(), lastCard, respState.score, leadState.score);
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

int GameState::playToTerminal(int attempts)
{
    while (!isTerminal())
    {
        int currAttempts = attempts;

        int idx;
        std::vector<int> valid = validActions();
        do
        {
            if (move.type == M_NONE) idx = leadState.player->getMove(valid);
            else idx = respState.player->getResponse(valid);
        }
        while (currAttempts-- && std::find(valid.begin(), valid.end(), idx) == valid.end());

        if (currAttempts < 0)
        { 
            int noActionPlayer = (move.type == M_NONE ? leadState : respState).mult;
            return noActionPlayer * -NOACTION_POINTS;
        }

        applyAction(idx);
    }

    return result();
}

int GameState::actionCode(int idx) const
{
    if (idx < 0) return idx;
    if (move.type == M_NONE)
    {
        Card card = leadState.hand[idx];
        if (trickNumber > 0 && isMarriageRank(card.rank) && leadState.isMarriageSuit[card.suit])
            return card.code() + NUM_SUITS * NUM_RANKS;
        return card.code();
    }
    else return respState.hand[idx].code();
}
