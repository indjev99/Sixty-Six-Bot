#pragma once

#include "card.h"
#include <vector>

bool leadWinsTrick(int trumpSuite, Card leadCard, Card respCard);
int findCard(std::string name, const std::vector<Card>& hand);
int findExchangeCard(int trumpSuite, const std::vector<Card>& hand);
std::vector<bool> findMarriageSuits(const std::vector<Card>& hand);
std::vector<int> findValidResponses(int trumpSuite, Card leadCard, const std::vector<Card>& hand);
