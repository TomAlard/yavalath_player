#ifndef YAVALATHCODINGAMEC_HEURISTICS_H
#define YAVALATHCODINGAMEC_HEURISTICS_H

#include <stdint.h>
#include "game.h"

typedef struct Heuristics Heuristics;

Heuristics* create_heuristics();

int heuristic_analysis(Game* game, Heuristics* heuristics, Coord last_move, uint8_t current_id, Coord* forced_move);

int get_position_heuristic(Heuristics* heuristics, Coord enemy_move);

#endif //YAVALATHCODINGAMEC_HEURISTICS_H
