#include "play_set.h"
#include "play_game.h"
#include "player_random.h"
#include "player_simple.h"
#include "player_human.h"
#include "rng.h"
#include <iostream>

double benchmark(int (play)(Player*, Player*), Player* leadPlayer, Player* respPlayer, bool alternate, int trials)
{
    int total = 0;
    for (int i = 0; i < trials; ++i)
    {
        if (alternate && i % 2 == 1) total -= playSet(respPlayer, leadPlayer);
        else total += playSet(leadPlayer, respPlayer);
    }
    return (double) total / trials;
}

int main()
{
    timeSeedRNG();

    PlayerRandom playerRandom;
    PlayerSimple playerSimple;
    PlayerHuman playerHuman;

    std::cout << "Average result: " << benchmark(playSet, &playerSimple, &playerRandom, true, 10000) << "." << std::endl;

    // int result = playSet(&playerHuman, &playerRandom);
    // std::cout << std::endl << "Set result is: " << result << "." << std::endl;

    return 0;
}
