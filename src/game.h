#ifndef YAVALATHCODINGAMEC_GAME_H
#define YAVALATHCODINGAMEC_GAME_H

#include <stdint.h>
#include "coord.h"
#include "board.h"

#define BOARD_SIZE 9
#define BOARD_SQUARES 61
#define WIN 10000
#define LOSS -WIN
#define INF (WIN+1)

typedef unsigned long long Hash;

typedef struct Game Game;

Game* init_game();

void free_game(Game* game);

int init_position(Game* game, char* filename);

Board* get_board(Game* game);

int8_t get_id_of_move(Game* game, Coord coord);

void make_move(Game* game, Coord coord, uint8_t current_id);

void undo_move(Game* game, Coord coord);

int iterate(Game* game, Coord moves[BOARD_SQUARES], uint8_t current_id, int heuristic_value, bool sort_moves);

int get_true_value_of_game(Game* game, Coord last_move);

int get_heuristic_value_of_game(Game* game, Coord last_move, int current_heuristic_value);

Coord get_forced_move_in_position(Game* game);

int get_position_heuristic_of_move(Game* game, Coord move);

void play_move(Game* game, uint8_t my_id, Coord move, char* message, bool print);

#endif //YAVALATHCODINGAMEC_GAME_H
