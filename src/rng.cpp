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
