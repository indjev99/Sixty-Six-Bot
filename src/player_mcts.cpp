#include "player_mcts.h"
#include "mcts.h"
#include "config.h"
#include "util.h"
#include "rng.h"
#include <algorithm>

void PlayerMCTS::startSet() {}

void PlayerMCTS::startGame()
{
    trickNumber = 0;
    closed = false;
    lastMove.type = M_NONE;

    selfState = PlayerGameState(1, nullptr);
    oppState = PlayerGameState(-1, nullptr);

    unseenCards.clear();
    knownOppCards.clear();

    for (int rank = 0; rank < NUM_RANKS; ++rank)
    {
        for (int suit = 0; suit < NUM_SUITS; ++suit)
        {
            unseenCards.push_back({rank, suit});
        }
    }
}

void PlayerMCTS::giveState(bool closed, int talonSize, Card trumpCard, int selfScore, int oppScore)
{
    if (trickNumber == 0) trumpSuit = trumpCard.suit;
    this->closed = closed;
    this->talonSize = talonSize;
    selfState.score = selfScore;
    oppState.score = oppScore;

    if (!closed && talonSize > 0)
    {
        lastTrumpCard = trumpCard;
        unseenCards.erase(std::remove(unseenCards.begin(), unseenCards.end(), lastTrumpCard), unseenCards.end());
    }
}

void PlayerMCTS::giveHand(const std::vector<Card>& hand)
{
    this->hand = hand;
    selfState.hand = hand;

    for (Card card : hand)
    {
        unseenCards.erase(std::remove(unseenCards.begin(), unseenCards.end(), card), unseenCards.end());
    }
}

void PlayerMCTS::giveMove(Move move)
{
    if (move.type == M_CLOSE) oppState.hasClosed = true;
    else if (move.type == M_EXCHANGE) knownOppCards.push_back(lastTrumpCard);
    else if (move.type == M_PLAY)
    {
        lastMove = move;

        unseenCards.erase(std::remove(unseenCards.begin(), unseenCards.end(), move.card), unseenCards.end());
        knownOppCards.erase(std::remove(knownOppCards.begin(), knownOppCards.end(), move.card), knownOppCards.end());
        if (move.score != 0)
        {
            oppState.score += move.score;
            for (int rank : MARRIAGE_RANKS)
            {
                if (rank == move.card.rank) continue;
                Card card(rank, move.card.suit);
                unseenCards.erase(std::remove(unseenCards.begin(), unseenCards.end(), card), unseenCards.end());
                knownOppCards.push_back(card);
            }
        }
    }
}

// TODO: deduce info from responses after closing

void PlayerMCTS::giveResponse(Card card)
{
    if (leadWinsTrick(trumpSuit, lastMove.card, card))
    {
        selfState.hasTakenTricks = true;
        if (talonSize == 2) knownOppCards.push_back(lastTrumpCard);
    }
    else oppState.hasTakenTricks = true;

    unseenCards.erase(std::remove(unseenCards.begin(), unseenCards.end(), card), unseenCards.end());
    knownOppCards.erase(std::remove(knownOppCards.begin(), knownOppCards.end(), card), knownOppCards.end());

    ++trickNumber;
    lastMove.type = M_NONE;
}

void PlayerMCTS::giveGameResult(int newPoints, int selfPoints, int oppPoints) {}
void PlayerMCTS::giveSetResult(int result) {}

int PlayerMCTS::getMove(const std::vector<int>& valid)
{
    int move = getAction();

    if (move >= 0)
    {
        lastMove.type = M_PLAY;
        lastMove.card = hand[move];
    }
    else if (move == M_CLOSE) selfState.hasClosed = true;

    return move;
}

int PlayerMCTS::getResponse(const std::vector<int>& valid)
{
    int response = getAction();

    Card card = hand[response];

    if (leadWinsTrick(trumpSuit, lastMove.card, card)) oppState.hasTakenTricks = true;
    else
    {
        selfState.hasTakenTricks = true;
        if (talonSize == 2) knownOppCards.push_back(lastTrumpCard);
    }

    ++trickNumber;
    lastMove.type = M_NONE;

    return response;
}

// TODO: remove
#include "printing.h"
#include <iostream>

bool toPrintDet;

GameState PlayerMCTS::determinize()
{
    int numOppCards = hand.size() - (lastMove.type != M_NONE);

    oppState.hand = knownOppCards;
    std::vector<Card> talon = unseenCards;

    if (talonSize > 0)
    {
        talon.insert(talon.begin(), lastTrumpCard);
        std::shuffle(talon.begin() + 1, talon.end(), RNG);
    }
    else std::shuffle(talon.begin(), talon.end(), RNG);

    while ((int) oppState.hand.size() < numOppCards)
    {
        oppState.hand.push_back(talon.back());
        talon.pop_back();
    }

    if (toPrintDet)
    {
        std::cerr << "Opponent hand:";
        std::sort(oppState.hand.begin(), oppState.hand.end());
        for (Card card : oppState.hand) { std::cerr << " "; printCard(card); }
        std::cerr << std::endl;

        std::cerr << "Talon:";
        for (Card card : talon) { std::cerr << " "; printCard(card); }
        std::cerr << std::endl;
    }

    if (lastMove.type == M_NONE) return GameState(trumpSuit, trickNumber, closed, lastMove, selfState, oppState, talon);
    else return GameState(trumpSuit, trickNumber, closed, lastMove, oppState, selfState, talon);
}

static const int NUM_DETERS = 5;
static const int NUM_ITERS = 10000;

int PlayerMCTS::getAction()
{
    // toPrintDet = true;
    MCTSNode node;
    std::vector<GameState> gameStates;
    for (int i = 0; i < NUM_DETERS; ++ i)
    {
        gameStates.push_back(determinize());
    }
    // toPrintDet = false;
    for (int i = 0; i < NUM_ITERS; ++i)
    {
        node.explore(gameStates[randInt(0, NUM_DETERS)]);
    }
    return node.choseAction(gameStates.front());
}
