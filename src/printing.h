#pragma once

#include "card.h"

#define NUM_COLORS 8
#define C_BLACK 0
#define C_BLUE 1
#define C_GREEN 2
#define C_CYAN 3
#define C_RED 4
#define C_MAGENTA 5
#define C_YELLOW 6
#define C_WHITE 7

extern bool FANCY_PRINTING;

void printCard(Card card);
void setColor(int color);
void resetColor();
void moveUpOneLine();
void waitForKeyPress();
