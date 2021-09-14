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

        double mean = (double) total / (i + 1);
        double meanSq = (double) totalSq / (i + 1);
        double std = sqrt((meanSq - mean * mean) / (i + 1));
        std::cout << i + 1 << ". " << mean << " +- " << std << std::endl;
    }
    double mean = (double) total / trials;
    double meanSq = (double) totalSq / trials;
    double std = sqrt((meanSq - mean * mean) / trials);
    return {mean, std};
}

PlayerUI playerHuman;

PlayerRandom playerRandom;
PlayerMCTS playerMCTSDumb(300, 2, false);
PlayerSimple playerSimple;
PlayerMCTS playerMCTSLight(600);
PlayerMCTS playerMCTSMid(5000);
PlayerMCTS playerMCTSHeavy(40000);

PlayerRandom playerRandomClone;
PlayerMCTS playerMCTSDumbClone(300, 2, false);
PlayerSimple playerSimpleClone;
PlayerMCTS playerMCTSLightClone(600);
PlayerMCTS playerMCTSMidClone(5000);
PlayerMCTS playerMCTSHeavyClone(40000);

std::vector<Player*> playerBots = {
    &playerRandom, &playerMCTSDumb, &playerSimple, &playerMCTSLight, &playerMCTSMid, &playerMCTSHeavy
};

std::vector<Player*> playerBotClones = {
    &playerRandomClone, &playerMCTSDumbClone, &playerSimpleClone, &playerMCTSLightClone, &playerMCTSMidClone, &playerMCTSHeavyClone
};

Player* choosePlayer(bool opponent)
{
    int idx = playerBots.size();
    std::string word = opponent ? "opponent" : "PoV";
    while (idx < 0 || idx >= (int) playerBots.size())
    {
        std::cout << "Choose " << word << " strength (0 - " << playerBots.size() - 1 << "): ";
        std::cin >> idx;
    }
    if (opponent) return playerBots[idx];
    else return playerBotClones[idx];
}

int main()
{
    timeSeedRNG();

    // std::pair<double, double> stats;
    // stats = benchmark(playGame, &playerMCTSDumb, &playerSimple, true, 1000);
    // std::cout << "Result: " << stats.first << " +- " << stats.second << "." << std::endl;

    while (true)
    {
        std::string command;
        std::cout << std::endl;
        std::cout << "Play, obeserve, benchmark, settings or exit: ";
        std::cin >> command;

        if (stringMatch(command, "Play"))
        {
            Player* playerLead = &playerHuman;
            Player* playerResp = choosePlayer(true);
            if (randInt(0, 2)) std::swap(playerLead, playerResp);
            playSet(playerLead, playerResp);
        }
        else if (stringMatch(command, "Observe"))
        {
            PlayerUI playerPoV(choosePlayer(false));
            Player* playerLead = &playerPoV;
            Player* playerResp = choosePlayer(true);
            if (randInt(0, 2)) std::swap(playerLead, playerResp);
            playSet(playerLead, playerResp);
        }
        else if (stringMatch(command, "Benchmark"))
        {
            int numTrials;
            std::cout << "Number of trials: ";
            std::cin >> numTrials;
            Player* playerPoV = choosePlayer(false);
            Player* playerOpponent = choosePlayer(true);
            std::pair<double, double> stats = benchmark(playSet, playerPoV, playerOpponent, true, numTrials);
            std::cout << "Result: " << stats.first << " +- " << stats.second << "." << std::endl;
        }
        if (stringMatch(command, "Settings"))
        {
            std::cout << "Enable fancy printing (0/1)? ";
            std::cin >> FANCY_PRINTING;
        }
        if (stringMatch(command, "Exit")) break;
    }

    return 0;
}
