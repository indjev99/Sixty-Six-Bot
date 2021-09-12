#pragma once

#include "card.h"
#include <vector>

bool stringMatch(const std::string& left, const std::string& right);
int findCard(const std::string& name, const std::vector<Card>& hand);

bool leadWinsTrick(int trumpSuit, Card leadCard, Card respCard);
int findExchangeCard(int trumpSuit, const std::vector<Card>& hand);
bool isMarriageCard(Card card);
std::vector<bool> findMarriageSuits(const std::vector<Card>& hand);
std::vector<int> findValidResponses(int trumpSuit, Card leadCard, const std::vector<Card>& hand);
