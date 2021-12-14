#ifndef YAVALATHCODINGAMEC_BOARD_H
#define YAVALATHCODINGAMEC_BOARD_H

#include <stdint.h>
#include "coord.h"

#define BOARD_SIZE 9
#define BOARD_SQUARES 61
#define WIN 10000
#define LOSS -WIN
#define INF (WIN+1)

typedef unsigned long long Hash;

typedef struct Board Board;

Board* init_board();

void free_board(Board* board);

int init_position(Board* board, char* filename);

int8_t get_id(Board* board, Coord coord);

void make_move(Board* board, Coord coord, uint8_t current_id);

void undo_move(Board* board, Coord coord);

int iterate(Board* board, Coord moves[BOARD_SQUARES], uint8_t current_id, int heuristic_value, bool sort_moves);

int get_true_value_of_board(Board* board, Coord last_move);

int get_heuristic_value_of_board(Board* board, Coord last_move, int current_heuristic_value);

Coord get_forced_move_in_position(Board* board);

int get_position_heuristic_of_move(Board* board, Coord move);

void play_move(Board* board, uint8_t my_id, Coord move, char* message, bool print);

#endif //YAVALATHCODINGAMEC_BOARD_H
