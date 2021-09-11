#pragma once

#include <random>

extern std::mt19937 RNG;

void seedRNG(int seed);
void timeSeedRNG();
int randInt(int from, int to);
