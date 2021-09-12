#include "play_set.h"
#include "play_game.h"
#include "player_random.h"
#include "player_human.h"
#include "rng.h"
#include <iostream>

int main()
{
    timeSeedRNG();

    PlayerRandom playerRandom;
    PlayerHuman playerHuman;

    int result = playSet(&playerHuman, &playerRandom);

    std::cout << std::endl << "Set result is: " << result << "." << std::endl;

    return 0;
}
