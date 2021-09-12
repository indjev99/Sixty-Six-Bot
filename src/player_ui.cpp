#include "player_ui.h"
#include "config.h"
#include "util.h"
#include <iostream>

PlayerUI::PlayerUI():
    player(nullptr) {}

PlayerUI::PlayerUI(Player* player):
    player(player) {}

void PlayerUI::startSet()
{
    if (player) player->startSet();

    std::cout << std::endl;
    std::cout << "Starting new set." << std::endl;
}

void PlayerUI::startGame()
{
    if (player) player->startGame();
    trickNumber = 0;

    std::cout << std::endl;
    std::cout << "Starting new game." << std::endl;
}

void PlayerUI::giveState(bool closed, int talonSize, Card trumpCard, int selfScore, int oppScore)
{
    if (player) player->giveState(closed, talonSize, trumpCard, selfScore, oppScore);
    if (trickNumber == 0) trumpSuit = trumpCard.suit;

    std::cout << std::endl;
    std::cout << "Your score is " << selfScore << ".";
    std::cout << " | Opponent's score is " << oppScore << ".";
    if (closed) std::cout << " | Talon is closed.";
    else if (talonSize == 0) std::cout << " | Talon is depleted.";
    else
    {
        std::cout << " | Talon has " << talonSize << " cards.";
        std::cout << " | Last card is " << trumpCard.toString() << ".";
    }
    std::cout << std::endl;
}

void PlayerUI::giveHand(const std::vector<Card>& hand)
{
    if (player) player->giveHand(hand);
    this->hand = hand;

    std::cout << std::endl;
    std::cout << "Hand:";
    for (Card card : hand)
    {
        std::cout << " " << card.toString();
    }
    std::cout << "." << std::endl;
}

void PlayerUI::giveMove(Move move)
{
    if (player) player->giveMove(move);

    if (move.type == M_PLAY)
    {
        std::cout << "Opponent played " << move.card.toString();
        if (move.score != 0) std::cout << " with " << move.score;
        std::cout << "." << std::endl;
    }
    else if (move.type == M_CLOSE)
    {
        std::cout << "Opponent closed the talon." << std::endl;
    }
    else if (move.type == M_EXCHANGE)
    {
        std::cout << "Opponent exchanged the face up trump card." << std::endl;
    }
}

void PlayerUI::giveResponse(Card card)
{
    if (player) player->giveResponse(card);

    std::cout << "Opponent responded with " << card.toString() << "." << std::endl;
}

static std::string pointsWord(int points)
{
    return points == 1 ? "point" : "points";
}

void PlayerUI::giveGameResult(int newPoints, int selfPoints, int oppPoints)
{
    if (player) player->giveGameResult(newPoints, selfPoints, oppPoints);

    std::cout << std::endl;
    if (newPoints == 0) std::cout << "Draw.";
    else
    {
        std::string doer = newPoints > 0 ? "You" : "Opponent";
        std::cout << doer << " won " << std::abs(newPoints) << " " << pointsWord(std::abs(newPoints)) << ".";
    }
    std::cout << " | You have " << selfPoints << " " << pointsWord(selfPoints) << ".";
    std::cout << " | Opponent has " << oppPoints << " " << pointsWord(oppPoints) << ".";
    std::cout << std::endl;
}

void PlayerUI::giveSetResult(int result)
{
    if (player) player->giveSetResult(result);

    std::cout << std::endl;
    std::string doer = result > 0 ? "You" : "Opponent";
    std::cout << doer << " won the set." << std::endl;

}

int PlayerUI::getMove()
{
    if (player)
    {
        int move = player->getMove();
        if (move >= 0)
        {
            int moveScore = 0;
            std::vector<bool> marriageSuits = findMarriageSuits(hand);
            if (trickNumber > 0 && marriageSuits[hand[move].suit] && isMarriageCard(hand[move]))
            {
                moveScore = hand[move].suit == trumpSuit ? TRUMP_MARRIAGE_VALUE : REG_MARRIAGE_VALUE;
            }

            std::cout << "You played " << hand[move].toString();
            if (moveScore != 0) std::cout << " with " << moveScore;
            std::cout << "." << std::endl;
        }
        else if (move == M_CLOSE)
        {
            std::cout << "You closed the talon." << std::endl;
        }
        else if (move == M_EXCHANGE)
        {
            std::cout << "You exchanged the face up trump card." << std::endl;
        }
        return move;
    }
    
    std::string move;
    std::cout << "Select move: ";
    std::cin >> move;
    if (move == "Close") return M_CLOSE;
    if (move == "Exchange") return M_EXCHANGE;
    return findCard(move, hand);
}

int PlayerUI::getResponse(const std::vector<int>& valid)
{
    ++trickNumber;

    if (player)
    {
        int response = player->getResponse(valid);
        std::cout << "You responded with " << hand[response].toString() << "." << std::endl;
        return response;
    }

    std::string response;
    std::cout << "Select response: ";
    std::cin >> response;
    return findCard(response, hand);
}
