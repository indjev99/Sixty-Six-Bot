#include "external_source_ui.h"
#include "printing.h"
#include "config.h"
#include "util.h"
#include <iostream>

Move stringToMove(const std::string& moveStr)
{
    Move move;

    move.type = M_CLOSE;
    if (stringMatch(moveStr, move.toString())) return move;

    move.type = M_EXCHANGE;
    if (stringMatch(moveStr, move.toString())) return move;

    move.type = M_PLAY;
    for (int code = 0; code < NUM_SUITS * NUM_RANKS; ++code)
    {
        move.score = 0;
        move.card = Card(code);
        if (stringMatch(moveStr, move.toString())) return move;

        if (!isMarriageRank(move.card.rank)) continue;

        move.score = 1;
        if (stringMatch(moveStr, move.toString())) return move;
    }

    return Move();
}

Move inputMove()
{
    std::string moveStr;
    Move move;
    do
    {
        std::cin >> moveStr;
        move = stringToMove(moveStr);
    }
    while (move.type == M_NONE);
    return move;
}

Card inputCard()
{
    Move cardMove;
    do
    {
        cardMove = inputMove();
    }
    while (cardMove.type != M_PLAY || cardMove.score);
    return cardMove.card;
}

ExternalSourceUI::ExternalSourceUI(bool printing):
    printing(printing) {}

bool ExternalSourceUI::getLeading()
{
    std::cout << std::endl;
    std::cout << "Leading (0/1): ";

    bool leading;
    std::cin >> leading;
    moveUpOneLine();

    if (printing)
    {
        if (leading) std::cout << "You are leading" << std::endl;
        else std::cout << "Opponent is leading" << std::endl;
    }
    else moveUpOneLine();

    return leading;
}

Card ExternalSourceUI::getTrumpCard()
{
    if (!printing) std::cout << std::endl;
    std::cout << "Trump card: ";

    Card trumpCard = inputCard();
    moveUpOneLine();

    if (printing)
    {
        std::cout << "Trump card: ";
        printCard(trumpCard);
        std::cout << std::endl;
    }
    else moveUpOneLine();

    return trumpCard;
}

std::vector<Card> ExternalSourceUI::getHand()
{
    if (!printing) std::cout << std::endl;
    std::cout << "Hand: ";

    std::vector<Card> hand(HAND_SIZE);
    for (Card& card : hand)
    {
        card = inputCard();
    }
    moveUpOneLine();

    if (printing)
    {
        std::cout << "Hand:";
        for (Card& card : hand)
        {
            std::cout << " ";
            printCard(card);
        }
        std::cout << std::endl;
    }
    else moveUpOneLine();

    return hand;
}

Card ExternalSourceUI::getDrawnCard()
{
    if (!printing) std::cout << std::endl;
    std::cout << "Drawn card: ";

    Card drawnCard = inputCard();
    moveUpOneLine();

    if (printing)
    {
        std::cout << "Drawn card: ";
        printCard(drawnCard);
        std::cout << std::endl;
    }
    else moveUpOneLine();

    return drawnCard;
}

Move ExternalSourceUI::getMove()
{
    if (!printing) std::cout << std::endl;
    std::cout << "Opponent's move: ";

    Move move = inputMove();
    moveUpOneLine();

    if (printing)
    {
        std::cout << "Opponent's move: ";
        printMove(move);
        std::cout << std::endl;
    }
    else moveUpOneLine();

    return move;
}

void ExternalSourceUI::giveMove(Move move)
{
    if (!printing) return;
    std::cout << "Your move: ";
    printMove(move);
    std::cout << std::endl;
}
