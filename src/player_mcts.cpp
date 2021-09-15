#include "player_mcts.h"
#include "mcts.h"
#include "config.h"
#include "util.h"
#include "rng.h"
#include <algorithm>

PlayerMCTS::PlayerMCTS(int numPlayouts, int numDeterms, bool countCards):
    numPlayouts(numPlayouts),
    numDeterms(numDeterms),
    countCards(countCards) {}

void PlayerMCTS::startSet()
{
    selfPoints = 0;
    oppPoints = 0;
}

void PlayerMCTS::startGame()
{
    trickNumber = 0;
    closed = false;
    lastMove.type = M_NONE;

    selfState = PlayerGameState(1, nullptr);
    oppState = PlayerGameState(-1, nullptr);

    unseenCards.clear();
    knownOppCards.clear();
    knownTalonCards.clear();

    for (int rank = 0; rank < NUM_RANKS; ++rank)
    {
        for (int suit = 0; suit < NUM_SUITS; ++suit)
        {
            unseenCards.push_back({rank, suit});
        }
    }
}

void PlayerMCTS::setCardKnownOpp(Card card)
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

    if (move.type == M_EXCHANGE) setCardKnownOpp(lastTrumpCard);
    else if (move.type == M_PLAY)
    {
        unseenCards.erase(std::remove(unseenCards.begin(), unseenCards.end(), move.card), unseenCards.end());
        knownOppCards.erase(std::remove(knownOppCards.begin(), knownOppCards.end(), move.card), knownOppCards.end());
        if (move.score != 0)
        {
            for (int rank : MARRIAGE_RANKS)
            {
                if (rank == move.card.rank) continue;
                setCardKnownOpp(Card(rank, move.card.suit));
            }
        }
    }
}

void PlayerMCTS::giveResponse(Card card, bool self)
{
    PlayerGameState& leadState = self ? oppState : selfState;
    PlayerGameState& respState = self ? selfState : oppState;

    if (leadWinsTrick(trumpSuit, lastMove.card, card)) leadState.hasTakenTricks = true;
    else respState.hasTakenTricks = true;

    if (!closed && talonSize == 2 && leadWinsTrick(trumpSuit, lastMove.card, card) != self) setCardKnownOpp(lastTrumpCard);

    ++trickNumber;
    lastMove.type = M_NONE;

    if (self) return;

    unseenCards.erase(std::remove(unseenCards.begin(), unseenCards.end(), card), unseenCards.end());
    knownOppCards.erase(std::remove(knownOppCards.begin(), knownOppCards.end(), card), knownOppCards.end());

    if (!closed) return;

    bool noRaises = card.suit == lastMove.card.suit && card.rank < lastMove.card.rank;
    bool noSuited = card.suit != lastMove.card.suit;
    bool noTrumps = card.suit != lastMove.card.suit && card.suit != trumpSuit;

    for (auto it = unseenCards.begin(); it != unseenCards.end();)
    {
        Card curr = *it;
        if ((noRaises && curr.suit == lastMove.card.suit && curr.rank > lastMove.card.rank) ||
            (noSuited && curr.suit == lastMove.card.suit) ||
            (noTrumps && curr.suit == trumpSuit))
        {
            knownTalonCards.push_back(curr);
            it = unseenCards.erase(it);
        }
        else ++it;
    }
}

void PlayerMCTS::giveGameResult(int newPoints, int selfPoints, int oppPoints)
{
    this->selfPoints = selfPoints;
    this->oppPoints = oppPoints;
}

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
    int oppHandSize = hand.size() - (lastMove.type != M_NONE);

    std::vector<Card> talon;

    if (countCards)
    {
        oppState.hand = knownOppCards;
        talon = unseenCards;

        if (talonSize > 0)
        {
            talon.insert(talon.begin(), lastTrumpCard);
            std::shuffle(talon.begin() + 1, talon.end(), RNG);
        }
        else std::shuffle(talon.begin(), talon.end(), RNG);

        int numMissing = oppHandSize - oppState.hand.size();
        oppState.hand.insert(oppState.hand.end(), talon.end() - numMissing, talon.end());
        talon.erase(talon.end() - numMissing, talon.end());

        talon.insert(talon.end(), knownTalonCards.begin(), knownTalonCards.end());
        if (talonSize > 0) std::shuffle(talon.begin() + 1, talon.end(), RNG);
        else std::shuffle(talon.begin(), talon.end(), RNG);
    }
    else
    {
        for (int code = 0; code < NUM_RANKS * NUM_SUITS; ++code)
        {
            talon.push_back(Card(code));
        }

        for (Card card : hand)
        {
            talon.erase(std::remove(talon.begin(), talon.end(), card), talon.end());
        }

        std::shuffle(talon.begin(), talon.end(), RNG);

        oppState.hand.clear();
        oppState.hand.insert(oppState.hand.end(), talon.end() - oppHandSize, talon.end());

        talon.resize(talonSize);
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
        node.explore(gameStates[randInt(0, numDeterms)], selfPoints, oppPoints);
    }

    // node.debug(gameStates.front());

    return node.choseAction(gameStates.front());
}
