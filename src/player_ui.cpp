#include "player_ui.h"
#include "config.h"
#include "printing.h"
#include "util.h"
#include <algorithm>
#include <iostream>
#include <conio.h>

static const int SUIT_COLORS[NUM_SUITS] = {C_BLUE, C_RED, C_GREEN, C_YELLOW};

static void printCard(Card card)
{
    if (FANCY_PRINTING)
    {
        setColor(SUIT_COLORS[card.suit]);
        std::cout << card.toString();
        resetColor();
    }
    else std::cout << card.toString();
}

static std::string pointsWord(int points)
{
    return points == 1 ? "point" : "points";
}

PlayerUI::PlayerUI():
    player(nullptr) {}

PlayerUI::PlayerUI(Player* player):
    player(player) {}

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

    this->closed = closed;
    this->talonSize = talonSize;
    if (trickNumber == 0) trumpSuit = trumpCard.suit;

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

    std::cout << std::endl;
    std::cout << "Hand:";
    for (Card card : hand)
    {
        std::cout << " ";
        printCard(card);
    }
    std::cout << "." << std::endl;
}

void PlayerUI::giveMove(Move move)
{
    if (player) player->giveMove(move);

    if (move.type == M_PLAY)
    {
        std::cout << "Opponent played ";
        printCard(move.card);
        if (move.score != 0) std::cout << " with " << move.score;
        std::cout << "." << std::endl;
    }
    else if (move.type == M_CLOSE) std::cout << "Opponent closed the talon." << std::endl;
    else if (move.type == M_EXCHANGE) std::cout << "Opponent exchanged the face up trump card." << std::endl;
}

void PlayerUI::giveResponse(Card card)
{
    if (player) player->giveResponse(card);

    std::cout << "Opponent responded with ";
    printCard(card);
    std::cout << "." << std::endl;
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
    getch();
}

void PlayerUI::giveSetResult(int result)
{
    if (player) player->giveSetResult(result);

    std::cout << std::endl;
    std::string doer = result > 0 ? "You" : "Opponent";
    std::cout << doer << " won the set." << std::endl;
    getch();
}

int PlayerUI::getMove()
{
    std::vector<int> moveScores(hand.size(), 0);
    std::vector<bool> marriageSuits = findMarriageSuits(hand);

    for (int i = 0; i < (int) hand.size(); ++i)
    {
        if (trickNumber > 0 && marriageSuits[hand[i].suit] && isMarriageCard(hand[i]))
        {
            moveScores[i] = hand[i].suit == trumpSuit ? TRUMP_MARRIAGE_VALUE : REG_MARRIAGE_VALUE;
        }
    }

    bool canDoTalonAct = !closed && trickNumber > 0 && talonSize >= TALON_ACT_TRESH;

    int move = hand.size();
    if (player) move = player->getMove();
    else
    {
        std::string moveStr;
        std::cout << "Select move: ";
        std::cin >> moveStr;
        if (FANCY_PRINTING) moveUpOneLine();

        if (stringMatch(moveStr, "Close")) move = M_CLOSE;
        else if (stringMatch(moveStr, "Exchange")) move = M_EXCHANGE;
        else move = findCard(moveStr, hand);
    }

    if (move >= 0 && move < (int) hand.size())
    {
        std::cout << "You played ";
        printCard(hand[move]);
        if (moveScores[move] != 0) std::cout << " with " << moveScores[move];
        std::cout << "." << std::endl;
    }
    else if (move == M_CLOSE && canDoTalonAct) std::cout << "You closed the talon." << std::endl;
    else if (move == M_EXCHANGE && canDoTalonAct && findExchangeCard(trumpSuit, hand) < (int) hand.size())
        std::cout << "You exchanged the face up trump card." << std::endl;

    if (player) getch();
    return move;
}

int PlayerUI::getResponse(const std::vector<int>& valid)
{
    ++trickNumber;

    int response = hand.size();
    if (player) response = player->getResponse(valid);
    else
    {
        std::string responseStr;
        std::cout << "Select response: ";
        std::cin >> responseStr;
        response = findCard(responseStr, hand);
        if (FANCY_PRINTING) moveUpOneLine();
    }

    if (std::find(valid.begin(), valid.end(), response) != valid.end())
    {
        std::cout << "You responded with ";
        printCard(hand[response]);
        std::cout << "." << std::endl;
    }

    if (player) getch();
    return response;
}
