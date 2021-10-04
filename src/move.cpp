#include "move.h"

Move::Move(int type, Card card, int score):
    type(type),
    card(card),
    score(score) {}

std::string Move::toString() const
{
    if (type == M_CLOSE) return "Close";
    if (type == M_EXCHANGE) return "Exchange";
    if (type == M_PLAY)
    {
        std::string cardStr = card.toString();
        if (score) return cardStr + "+";
        else return cardStr;
    }
    return "None";
}