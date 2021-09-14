#pragma once

#include "card.h"
#include <vector>

bool stringMatch(const std::string& left, const std::string& right);
int findCard(const std::string& name, const std::vector<Card>& hand);

bool leadWinsTrick(int trumpSuit, Card leadCard, Card respCard);
int findExchangeCard(int trumpSuit, const std::vector<Card>& hand);
bool isMarriageCard(int rank);
std::vector<bool> findMarriageSuits(const std::vector<Card>& hand);
