#include "player_mcts.h"
#include "mcts.h"
#include "ismcts.h"
#include "config.h"
#include "util.h"
#include "rng.h"
#include <algorithm>

PlayerMCTS::PlayerMCTS(int numPlayouts, bool infSets, int numOppDeterms, int numSelfRedeterms):
    numPlayouts(numPlayouts),
    infSets(infSets),
    numOppDeterms(numOppDeterms),
    numSelfRedeterms(numSelfRedeterms) {}

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

void PlayerMCTS::giveGameResult(int newPoints, int selfPoints, int oppPoints) {}
void PlayerMCTS::giveSetResult(int result) {}

int PlayerMCTS::getMove(const std::vector<int>& valid)
{
    int move = getAction(valid);
    return move;
}

int PlayerMCTS::getResponse(const std::vector<int>& valid)
{
    int response = getAction(valid);
    return response;
}

void PlayerMCTS::shuffleTalon()
{
    if (talonSize > 0) std::shuffle(talon.begin() + 1, talon.end(), RNG);
    else std::shuffle(talon.begin(), talon.end(), RNG);
}

void PlayerMCTS::setSelfHand(const std::vector<Card>& selfHand)
{
    selfState.hand = selfHand;

    talon = unseenCards;
    if (talonSize > 0) talon.insert(talon.begin(), lastTrumpCard);
    talon.insert(talon.end(), knownTalonCards.end(), knownTalonCards.begin());
    talon.insert(talon.end(), hand.begin(), hand.end());

    for (Card card : selfState.hand)
    {
        talon.erase(std::remove(talon.begin(), talon.end(), card), talon.end());
    }
    for (Card card : oppState.hand)
    {
        talon.erase(std::remove(talon.begin(), talon.end(), card), talon.end());
    }
    shuffleTalon();

    talon.resize(talonSize);
}

GameState PlayerMCTS::makeGameState()
{
    if (lastMove.type == M_NONE) return GameState(trumpSuit, trickNumber, closed, lastMove, selfState, oppState, talon);
    else return GameState(trumpSuit, trickNumber, closed, lastMove, oppState, selfState, talon);
}

void PlayerMCTS::determinizeOpponent()
{
    int oppHandSize = hand.size() - (lastMove.type != M_NONE);
    oppState.hand = unseenCards;
    std::shuffle(oppState.hand.begin(),  oppState.hand.end(), RNG);
    oppState.hand.resize(oppHandSize - knownOppCards.size());
    oppState.hand.insert(oppState.hand.end(), knownOppCards.begin(), knownOppCards.end());
}

void PlayerMCTS::redeterminizeSelf()
{
    // TODO: Improve by keeping track of opponent knowledge about self cards

    selfState.hand = unseenCards;
    selfState.hand.insert(selfState.hand.end(), hand.begin(), hand.end());
    std::shuffle(selfState.hand.begin(), selfState.hand.end(), RNG);
    selfState.hand.resize(hand.size());
}

int PlayerMCTS::getAction(const std::vector<int>& valid)
{
    int numActions = valid.size();
    std::vector<int> actionScores(numActions, 0);

    if (!infSets)
    {
        std::vector<GameState> gameStates;
        for (int i = 0; i < numOppDeterms; ++ i)
        {
            determinizeOpponent();
            setSelfHand(hand);
            gameStates.push_back(makeGameState());
        }

        MCTSNode node;
        for (int j = 0; j < numPlayouts; ++j)
        {
            node.explore(gameStates[randInt(0, numOppDeterms)]);
        }
        actionScores = node.scoreActions(gameStates.front(), valid);

        // node.debug(gameStates.front());
    }
    else
    {
        int currNumOppDeterms = talonSize > 0 ? numOppDeterms : 1;
        int currNumSelfRedeterms = talonSize > 0 ? numSelfRedeterms : 0;

        std::vector<std::vector<Card>> newSelfHands = {hand};
        for (int j = 0; j < currNumSelfRedeterms; ++j)
        {
            redeterminizeSelf();
            newSelfHands.push_back(selfState.hand);
        }

        std::vector<std::vector<GameState>> gameStates(currNumOppDeterms);
        for (int i = 0; i < currNumOppDeterms; ++ i)
        {
            determinizeOpponent();
            for (int j = 0; j < currNumSelfRedeterms + 1; ++j)
            {
                setSelfHand(newSelfHands[j]);
                gameStates[i].push_back(makeGameState());
            }
        }

        ISMCTSNode node(currNumSelfRedeterms + 1);
        for (int j = 0; j < numPlayouts; ++j)
        {
            int oppDeterm = randInt(0, currNumOppDeterms);
            int selfDeterm = randInt(0, currNumSelfRedeterms + 1);
            node.explore(gameStates[oppDeterm][selfDeterm], selfDeterm, selfDeterm, oppDeterm, currNumSelfRedeterms + 1, currNumOppDeterms);
        }
        actionScores = node.scoreActions(gameStates.front().front(), valid, 0);

        // node.debug(gameStates.front().front(), 0, 0, 0);
    }

    // else
    // {
    //     std::vector<GameState> gameStates;
    //     for (int i = 0; i < numDeterms; ++ i)
    //     {
    //         gameStates.push_back(determinize());
    //     }

    //     for (int i = 0; i < numDeterms; ++i)
    //     {
    //         MCTSNode node;
    //         for (int j = 0; j < numPlayouts / numDeterms; ++j)
    //         {
    //             node.explore(gameStates[i]);
    //         }
    //         std::vector<int> currScores = node.scoreActions(gameStates.front(), valid);

    //         for (int j = 0; j < numActions; ++j)
    //         {
    //             actionScores[j] += currScores[j];
    //         }
    //     }
    // }

    int maxIdx = numActions;
    for (int i = 0; i < numActions; ++i)
    {
        if (i == 0 || actionScores[i] > actionScores[maxIdx]) maxIdx = i;
    }
    return valid[maxIdx];
}
