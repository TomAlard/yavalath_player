#ifndef YAVALATHCODINGAMEC_TRANSPOSITION_TABLE_H
#define YAVALATHCODINGAMEC_TRANSPOSITION_TABLE_H

#include "game.h"

typedef struct TableEntry TableEntry;

TableEntry* create_transposition_table();

int get_true_value_from_table(TableEntry* table, Hash hash);

int get_heuristic_value_from_table(TableEntry* table, Hash hash);

Coord get_forced_move_from_table(TableEntry* table, Hash hash);

Coord get_book_move_from_table(TableEntry* table, Hash hash);

bool true_value_is_valid(int true_value);

bool heuristic_value_is_valid(int heuristic_value);

void add_true_value_to_table(TableEntry* table, Hash hash, int true_value);

void add_heuristic_value_to_table(TableEntry* table, Hash hash, int heuristic_value);

void add_forced_move_to_table(TableEntry* table, Hash hash, Coord forced_move);

void add_book_move_to_table(TableEntry* table, Hash hash, int8_t x, int8_t y);

#endif //YAVALATHCODINGAMEC_TRANSPOSITION_TABLE_H
