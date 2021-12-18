#ifndef YAVALATHCODINGAMEC_BOARD_H
#define YAVALATHCODINGAMEC_BOARD_H

#include "coord.h"
#include "game.h"

typedef struct Board Board;

Board* init_board();

int coord_to_index(Coord coord);

uint8_t get_id(Board* board, Coord coord);

void set_id(Board* board, Coord coord, uint8_t id);

int calculate_true_value(Board* board, Coord last_move);

#define FORCING 50
#define GREAT 20
#define BAD -10
int calculate_heuristic_value(Board* board, Coord last_move);

#endif //YAVALATHCODINGAMEC_BOARD_H
