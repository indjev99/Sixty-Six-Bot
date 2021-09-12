#include "play_set.h"
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

    int result = playSet(&humanPlayer, &randomPlayer);

    std::cout << std::endl << "Set result is: " << result << "." << std::endl;

    return 0;
}
