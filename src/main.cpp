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

    int result = playGame(&humanPlayer, &randomPlayer);
    std::cout << result << std::endl;

    return 0;
}
