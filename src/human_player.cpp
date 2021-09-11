#include "human_player.h"

#include <iostream>

void HumanPlayer::giveTrump(Card card)
{
    std::cout << "Face up trump card is " << card.toString() << "." << std::endl;
}

void HumanPlayer::giveHand(const std::vector<Card>& hand)
{
    std::cout << std::endl;
    std::cout << "Hand:";
    for (Card card : hand)
    {
        std::cout << " " << card.toString();
    }
    std::cout << "." << std::endl;
}

void HumanPlayer::giveMove(Move move, bool self)
{
    std::string doer = self ? "You" : "Opponent";
    if (move.type == M_PLAY)
    {
        std::cout << doer << " played " << move.card.toString();
        if (move.score != 0) std::cout << " with " << move.score;
        std::cout << "." << std::endl;
    }
    else if (move.type == M_CLOSE)
    {
        std::cout << doer << " closed the talon." << std::endl;
    }
    else if (move.type == M_EXCHANGE)
    {
        std::cout << doer << " exchanged the face up trump card." << std::endl;
    }
}

void HumanPlayer::giveResponse(Card card, bool self)
{
    std::string doer = self ? "You" : "Opponent";
    std::cout << doer << " responded with " << card.toString() << "." << std::endl;
}

void HumanPlayer::giveGameResult(int points)
{
    if (points == 0) std::cout << "Draw." << std::endl;
    else
    {
        std::string doer = points > 0 ? "You" : "Opponent";
        std::cout << doer << " won " << std::abs(points) << " points." << std::endl;
    }
}

int HumanPlayer::getMove()
{
    int move;
    std::cout << "Select move: ";
    std::cin >> move;
    return move;
}

int HumanPlayer::getResponse()
{
    int response;
    std::cout << "Select response: ";
    std::cin >> response;
    return response;
}
