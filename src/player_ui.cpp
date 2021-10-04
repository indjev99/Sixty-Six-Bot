#include "player_ui.h"
#include "config.h"
#include "printing.h"
#include "util.h"
#include <algorithm>
#include <iostream>

static std::string pointsWord(int points)
{
    return points == 1 ? "point" : "points";
}

PlayerUI::PlayerUI(Player* player, bool pause):
    player(player),
    pause(pause) {}

void PlayerUI::startSet()
{
    if (player) player->startSet();

    std::cout << std::endl;
    std::cout << "======================================================================================" << std::endl;
    std::cout << "Starting new set." << std::endl;
}

void PlayerUI::startGame()
{
    if (player) player->startGame();

    trickNumber = 0;

    std::cout << std::endl;
    std::cout << "--------------------------------------------------------------------------------------" << std::endl;
    std::cout << "Starting new game." << std::endl;
}

void PlayerUI::giveState(bool closed, int talonSize, Card trumpCard, int selfScore, int oppScore)
{
    if (player) player->giveState(closed, talonSize, trumpCard, selfScore, oppScore);

    if (trickNumber == 0) trumpSuit = trumpCard.suit;
    this->closed = closed;
    this->talonSize = talonSize;

    std::cout << std::endl;
    std::cout << "Your score is " << selfScore << ".";
    std::cout << " | Opponent's score is " << oppScore << ".";
    if (closed) std::cout << " | Talon is closed.";
    else if (talonSize == 0) std::cout << " | Talon is depleted.";
    else
    {
        std::cout << " | Talon has " << talonSize << " cards.";
        std::cout << " | Last card is ";
        printCard(trumpCard);
        std::cout << ".";
    }
    std::cout << std::endl;
}

void PlayerUI::giveHand(const std::vector<Card>& hand)
{
    if (player) player->giveHand(hand);

    this->hand = hand;

    std::vector<Card> sortedHand = hand;
    std::sort(sortedHand.begin(), sortedHand.end());

    std::cout << std::endl;
    std::cout << "Hand:";
    for (Card card : sortedHand)
    {
        std::cout << " ";
        printCard(card);
    }
    std::cout << "." << std::endl;
}

void PlayerUI::giveMove(Move move, bool self)
{
    if (player) player->giveMove(move, self);

    std::string doer = self ? "You" : "Opponent";
    if (move.type == M_PLAY)
    {
        std::cout << doer << " played ";
        printCard(move.card);
        if (move.score != 0) std::cout << " with " << move.score;
        std::cout << "." << std::endl;
    }
    else if (move.type == M_CLOSE) std::cout << doer << " closed the talon." << std::endl;
    else if (move.type == M_EXCHANGE) std::cout << doer << " exchanged the face up trump card." << std::endl;

    if (pause && move.type != M_PLAY) waitForKeyPress();
}

void PlayerUI::giveResponse(Card card, bool self)
{
    if (player) player->giveResponse(card, self);

    std::string doer = self ? "You" : "Opponent";
    std::cout << doer << " responded with ";
    printCard(card);
    std::cout << "." << std::endl;

    if (pause && player) waitForKeyPress();

    ++trickNumber;
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

    if (pause) waitForKeyPress();
}

void PlayerUI::giveSetResult(int result)
{
    if (player) player->giveSetResult(result);

    std::cout << std::endl;
    std::string doer = result > 0 ? "You" : "Opponent";
    std::cout << doer << " won the set." << std::endl;

    if (pause) waitForKeyPress();
}

int PlayerUI::getMove(const std::vector<int>& valid)
{
    int move = hand.size();
    if (player) move = player->getMove(valid);
    else
    {
        std::string moveStr;
        std::cout << "Select move: ";
        std::cin >> moveStr;
        moveUpOneLine();

        if (stringMatch(moveStr, "Close")) move = M_CLOSE;
        else if (stringMatch(moveStr, "Exchange")) move = M_EXCHANGE;
        else move = findCard(moveStr, hand);
    }
    return move;
}

int PlayerUI::getResponse(const std::vector<int>& valid)
{
    int response = hand.size();
    if (player) response = player->getResponse(valid);
    else
    {
        std::string responseStr;
        std::cout << "Select response: ";
        std::cin >> responseStr;
        response = findCard(responseStr, hand);
        moveUpOneLine();
    }
    return response;
}
