#pragma once

#include "card.h"
#include <vector>

bool leadWinsTrick(int trump, Card leadCard, Card respCard);
int findCard(std::string name, const std::vector<Card>& hand);
int findExchangeCard(int trump, const std::vector<Card>& hand);
std::vector<bool> findMarriageSuits(const std::vector<Card>& hand);
