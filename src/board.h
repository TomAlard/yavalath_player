#ifndef YAVALATHCODINGAMEC_BOARD_H
#define YAVALATHCODINGAMEC_BOARD_H

#include "coord.h"

typedef struct Board Board;

Board* init_board();

int8_t get_id(Board* board, Coord coord);

void set_id(Board* board, Coord coord, int8_t id);

void fill_pattern(Board* board, Coord coord, uint8_t direction, int8_t pattern[7]);

uint8_t calculate_line_length(Board* board, Coord start, uint8_t direction);

Coord get_forced_move_from_pattern(Board* board, const int8_t pattern[7], Coord last_move, int pattern_index, uint8_t direction);

#endif //YAVALATHCODINGAMEC_BOARD_H
