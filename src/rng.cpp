#include "rng.h"
#include "time.h"

std::mt19937 RNG;

void seedRNG(int seed)
{
    RNG.seed(seed);
}

void timeSeedRNG()
{
    seedRNG(time(nullptr));
}

int randInt(int from, int to)
{
    std::uniform_int_distribution<> distr(from, to - 1);
    return distr(RNG);
}
