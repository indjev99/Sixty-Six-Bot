#include "play_game.h"
#include "random_player.h"
#include "human_player.h"
#include "rng.h"
#include <iostream>

int main()
{
    timeSeedRNG();

    HumanPlayer humanPlayer;
    RandomPlayer randomPlayer;

    int points = playGame(&randomPlayer, &randomPlayer);

    std::cout << points << std::endl;

    std::cout << playGame(&randomPlayer, &randomPlayer) << std::endl;

    return 0;
}
