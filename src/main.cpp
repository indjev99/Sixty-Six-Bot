#include "play_set.h"
#include "play_game.h"
#include "player_random.h"
#include "player_simple.h"
#include "player_mcts.h"
#include "player_ui.h"
#include "config.h"
#include "util.h"
#include "rng.h"
#include <math.h>
#include <iostream>

std::pair<double, double> benchmark(int (play)(Player*, Player*), Player* leadPlayer, Player* respPlayer, bool alternate, int trials)
{
    int total = 0;
    int totalSq = 0;
    for (int i = 0; i < trials; ++i)
    {
        int res;
        if (alternate && i % 2 == 1) res = -play(respPlayer, leadPlayer);
        else res = play(leadPlayer, respPlayer);
        total += res;
        totalSq += res * res;
    }
    double mean = (double) total / trials;
    double meanSq = (double) totalSq / trials;
    double std = sqrt((meanSq - mean * mean) / trials);
    return {mean, std};
}

int main()
{
    timeSeedRNG();

    PlayerRandom playerRandom;
    PlayerSimple playerSimple;
    PlayerMCTS playerMCTS;
    PlayerUI playerHuman;

    PlayerSimple playerSimpleUnderlying;
    PlayerUI playerSimpleObserved(&playerSimpleUnderlying);

    PlayerMCTS playerMCTSUnderlying;
    PlayerUI playerMCTSObserved(&playerMCTSUnderlying);

    playSet(&playerMCTSObserved, &playerSimple);

    // std::pair<double, double> stats;
    // stats = benchmark(playGame, &playerMCTS, &playerRandom, true, 100);
    // std::cout << "Result: " << stats.first << " +- " << stats.second << "." << std::endl;

    while (true)
    {
        std::string command;
        std::cout << std::endl;
        std::cout << "Play, obeserve, settings or exit: ";
        std::cin >> command;

        if (stringMatch(command, "Play")) playSet(&playerHuman, &playerSimple);
        if (stringMatch(command, "Observe")) playSet(&playerSimpleObserved, &playerSimple);
        if (stringMatch(command, "Settings"))
        {
            std::cout << "Enable fancy printing (0/1)? ";
            std::cin >> FANCY_PRINTING;
        }
        if (stringMatch(command, "Exit")) break;
    }

    return 0;
}
