#include "human_player.h"
#include "config.h"
#include "util.h"
#include <iostream>

void HumanPlayer::reset()
{
    std::cout << std::endl;
    std::cout << "Starting new game." << std::endl;
}

void HumanPlayer::giveState(bool closed, int talonSize, Card trumpCard, int score, int oppScore)
{
    std::cout << std::endl;
    std::cout << "Your score is " << score << ".";
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

void HumanPlayer::giveHand(const std::vector<Card>& hand)
{
    this->hand = hand;

    std::cout << std::endl;
    std::cout << "Hand:";
    for (Card card : hand)
    {
        std::cout << " " << card.toString();
    }
    std::cout << "." << std::endl;
}

void HumanPlayer::giveMove(Move move)
{
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

void HumanPlayer::giveResponse(Card card)
{
    std::cout << "Opponent responded with " << card.toString() << "." << std::endl;
}

void HumanPlayer::giveGameResult(int points)
{
    std::cout << std::endl;
    if (points == 0) std::cout << "Draw." << std::endl;
    else
    {
        std::string doer = points > 0 ? "You" : "Opponent";
        std::cout << doer << " won " << std::abs(points) << " points." << std::endl;
    }
}

int HumanPlayer::getMove()
{
    std::string move;
    std::cout << "Select move: ";
    std::cin >> move;

    if (move == "Close") return M_CLOSE;
    if (move == "Exchange") return M_EXCHANGE;
    return findCard(move, hand);
}

int HumanPlayer::getResponse()
{
    std::string response;
    std::cout << "Select response: ";
    std::cin >> response;
    return findCard(response, hand);
}
