#include "player_mcts.h"
#include "mcts.h"
#include "config.h"
#include "util.h"
#include "rng.h"
#include <algorithm>

PlayerMCTS::PlayerMCTS(int numPlayouts, int numDeterms):
    numPlayouts(numPlayouts),
    numDeterms(numDeterms) {}

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

void PlayerMCTS::setCardKnown(Card card)
{
    unseenCards.erase(std::remove(unseenCards.begin(), unseenCards.end(), card), unseenCards.end());
    if (std::find(knownOppCards.begin(), knownOppCards.end(), card) == knownOppCards.end()) knownOppCards.push_back(card);
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

void PlayerMCTS::giveMove(Move move, bool self)
{
    PlayerGameState& leadState = self ? selfState : oppState;
    if (move.type == M_CLOSE) leadState.hasClosed = true;
    else if (move.type == M_PLAY)
    {
        lastMove = move;
        leadState.score += move.score;
    }

    if (self) return;

    if (move.type == M_EXCHANGE) setCardKnown(lastTrumpCard);
    else if (move.type == M_PLAY)
    {
        unseenCards.erase(std::remove(unseenCards.begin(), unseenCards.end(), move.card), unseenCards.end());
        knownOppCards.erase(std::remove(knownOppCards.begin(), knownOppCards.end(), move.card), knownOppCards.end());
        if (move.score != 0)
        {
            for (int rank : MARRIAGE_RANKS)
            {
                if (rank == move.card.rank) continue;
                setCardKnown(Card(rank, move.card.suit));
            }
        }
    }
}

// TODO: deduce info from responses after closing

void PlayerMCTS::giveResponse(Card card, bool self)
{
    PlayerGameState& leadState = self ? oppState : selfState;
    PlayerGameState& respState = self ? selfState : oppState;

    if (leadWinsTrick(trumpSuit, lastMove.card, card)) leadState.hasTakenTricks = true;
    else respState.hasTakenTricks = true;

    if (!closed && talonSize == 2 && leadWinsTrick(trumpSuit, lastMove.card, card) != self) setCardKnown(lastTrumpCard);

    ++trickNumber;
    lastMove.type = M_NONE;

    if (self) return;

    unseenCards.erase(std::remove(unseenCards.begin(), unseenCards.end(), card), unseenCards.end());
    knownOppCards.erase(std::remove(knownOppCards.begin(), knownOppCards.end(), card), knownOppCards.end());
}

void PlayerMCTS::giveGameResult(int newPoints, int selfPoints, int oppPoints) {}
void PlayerMCTS::giveSetResult(int result) {}

int PlayerMCTS::getMove(const std::vector<int>& valid)
{
    int move = getAction();
    return move;
}

int PlayerMCTS::getResponse(const std::vector<int>& valid)
{
    int response = getAction();
    return response;
}

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

    if (lastMove.type == M_NONE) return GameState(trumpSuit, trickNumber, closed, lastMove, selfState, oppState, talon);
    else return GameState(trumpSuit, trickNumber, closed, lastMove, oppState, selfState, talon);
}

int PlayerMCTS::getAction()
{
    MCTSNode node;
    std::vector<GameState> gameStates;
    for (int i = 0; i < numDeterms; ++ i)
    {
        GameState gs = determinize();
        gameStates.push_back(gs);
    }
    for (int i = 0; i < numPlayouts; ++i)
    {
        node.explore(gameStates[randInt(0, numDeterms)]);
    }
    return node.choseAction(gameStates.front());
}
