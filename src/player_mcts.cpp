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

void PlayerMCTS::giveMove(Move move)
{
    if (move.type == M_CLOSE) oppState.hasClosed = true;
    else if (move.type == M_EXCHANGE) setCardKnown(lastTrumpCard);
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
                setCardKnown(Card(rank, move.card.suit));
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
        if (talonSize == 2) setCardKnown(lastTrumpCard);
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
        if (talonSize == 2) setCardKnown(lastTrumpCard);
    }

    ++trickNumber;
    lastMove.type = M_NONE;

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
