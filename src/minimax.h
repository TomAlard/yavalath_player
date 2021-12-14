#ifndef YAVALATHCODINGAMEC_MINIMAX_H
#define YAVALATHCODINGAMEC_MINIMAX_H

#include <time.h>
#include "board.h"

int calculate_minimax_value(Board* board, Coord last_move, int max_depth, clock_t deadline, int heuristic_value, Coord* best_move);

#endif //YAVALATHCODINGAMEC_MINIMAX_H
