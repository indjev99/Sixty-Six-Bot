#include "play_set.h"
#include "play_game.h"
#include "player_random.h"
#include "player_simple.h"
#include "player_mcts.h"
#include "player_ui.h"
#include "external_source_ui.h"
#include "printing.h"
#include "util.h"
#include "rng.h"
#include <math.h>
#include <iostream>

template <typename T>
std::pair<double, double> benchmark(T (play)(Player*, Player*), Player* leadPlayer, Player* respPlayer, bool alternate, int trials)
{
    double total = 0;
    double totalSq = 0;
    for (int i = 0; i < trials; ++i)
    {
        double res;
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
PlayerSimple playerSimple;
PlayerMCTS playerMCTSLight(600, 10, 0);
PlayerMCTS playerMCTSMid(5000, 10, 10, 0.5);
PlayerMCTS playerMCTSHeavy(40000, 40, 40, 0.75);
PlayerMCTS playerMCTSUltra(300000, 40, 40, 0.85);

PlayerRandom playerRandomClone;
PlayerSimple playerSimpleClone;
PlayerMCTS playerMCTSLightClone(600, 10, 0);
PlayerMCTS playerMCTSMidClone(5000, 10, 10, 0.5);
PlayerMCTS playerMCTSHeavyClone(40000, 40, 40, 0.75);
PlayerMCTS playerMCTSUltraClone(300000, 40, 40, 0.85);

std::vector<Player*> playerBots = {
    &playerRandom, &playerSimple, &playerMCTSLight, &playerMCTSMid, &playerMCTSHeavy, &playerMCTSUltra
};

std::vector<Player*> playerBotClones = {
    &playerRandomClone, &playerSimpleClone, &playerMCTSLightClone, &playerMCTSMidClone, &playerMCTSHeavyClone, &playerMCTSUltraClone
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

double playGameHedged(Player* leadPlayer, Player* respPlayer)
{
    GameState gameState(leadPlayer, respPlayer);
    GameState gameStateBase = gameState;

    int res = gameState.playToTerminal(5);

    gameStateBase.setPlayers(&playerMCTSLight, &playerMCTSLightClone);
    int base = gameStateBase.playToTerminal(5);

    return res - base * 0.5;
}

int main()
{
    timeSeedRNG();

    // PlayerMCTS playerMCTSTest(5000, 10, 10, 0.5, true);

    // PlayerUI povPlayer(&playerMCTSTest);
    // playGame(&playerMCTSMidClone, &povPlayer);

    // std::pair<double, double> stats;
    // stats = benchmark(playGameHedged, &playerMCTSTest, &playerMCTSMidClone, true, 50000);
    // std::cout << "Result: " << stats.first << " +- " << stats.second << "." << std::endl;

    while (true)
    {
        std::string command;
        std::cout << std::endl;
        std::cout << "Play, obeserve, external, benchmark, settings or exit: ";
        std::cin >> command;

        if (stringMatch(command, "Play"))
        {
            Player* leadPlayer = &playerHuman;
            Player* respPlayer = choosePlayer(true);
            if (randInt(0, 2)) std::swap(leadPlayer, respPlayer);
            playSet(leadPlayer, respPlayer);
        }
        else if (stringMatch(command, "Observe"))
        {
            PlayerUI povPlayer(choosePlayer(false));
            Player* leadPlayer = &povPlayer;
            Player* respPlayer = choosePlayer(true);
            if (randInt(0, 2)) std::swap(leadPlayer, respPlayer);
            playSet(leadPlayer, respPlayer);
        }
        else if (stringMatch(command, "External"))
        {
            ExternalSourceUI extSourceUI(false);
            PlayerUI povPlayer(choosePlayer(false), false);
            playSetExternal(&povPlayer, &extSourceUI);
        }
        else if (stringMatch(command, "Benchmark"))
        {
            int numTrials;
            std::cout << "Number of trials: ";
            std::cin >> numTrials;
            Player* povPlayer = choosePlayer(false);
            Player* oppPlayer = choosePlayer(true);
            std::pair<double, double> stats = benchmark(playSet, povPlayer, oppPlayer, true, numTrials);
            std::cout << "Result: " << stats.first << " +- " << stats.second << "." << std::endl;
        }
        else if (stringMatch(command, "Settings"))
        {
            std::cout << "Enable fancy printing (0/1)? ";
            std::cin >> FANCY_PRINTING;
        }
        else if (stringMatch(command, "Exit")) break;
    }

    return 0;
}
