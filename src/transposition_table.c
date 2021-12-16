#include "transposition_table.h"
#include "util.h"


typedef struct TableEntry {
    Hash key;
    short true_value;
    short heuristic_value;
    Coord forced_move;
    Coord book_move;
} TableEntry;


#define TRANSPOSITION_TABLE_SIZE 67108864  // 2^26
TableEntry* create_transposition_table() {
    TableEntry* result = safe_malloc(TRANSPOSITION_TABLE_SIZE * sizeof(TableEntry));
    for (int i = 0; i < TRANSPOSITION_TABLE_SIZE; i++) {
        TableEntry* entry = &result[i];
        set_coord_invalid(&entry->forced_move);
        entry->key = 0;
        entry->true_value = -1;
        entry->heuristic_value = INF;
    }
    return result;
}


Hash get_index(Hash hash) {
    return hash & ((1 << 26) - 1);
}


int get_true_value_from_table(TableEntry* table, Hash hash) {
    Hash index = get_index(hash);
    return table[index].key == hash? table[index].true_value : -1;
}


int get_heuristic_value_from_table(TableEntry* table, Hash hash) {
    Hash index = get_index(hash);
    return table[index].key == hash? table[index].heuristic_value : INF;
}


Coord get_forced_move_from_table(TableEntry* table, Hash hash) {
    Hash index = get_index(hash);
    Coord invalid = INVALID_COORD;
    return table[index].key == hash? table[index].forced_move : invalid;
}


Coord get_book_move_from_table(TableEntry* table, Hash hash) {
    Hash index = get_index(hash);
    Coord invalid = INVALID_COORD;
    return table[index].key == hash? table[index].book_move : invalid;
}


bool true_value_is_valid(int true_value) {
    return true_value != -1;
}


bool heuristic_value_is_valid(int heuristic_value) {
    return heuristic_value != INF;
}


void clear_entry_if_different_hash(TableEntry* table, Hash hash, int index) {
    if (table[index].key != hash) {
        set_coord_invalid(&table[index].forced_move);
        table[index].heuristic_value = INF;
        table[index].true_value = -1;
        set_coord_invalid(&table[index].book_move);
    }
}


void add_true_value_to_table(TableEntry* table, Hash hash, int true_value) {
    Hash index = get_index(hash);
    clear_entry_if_different_hash(table, hash, index);
    table[index].true_value = (short) true_value;
    table[index].key = hash;
}


void add_heuristic_value_to_table(TableEntry* table, Hash hash, int heuristic_value) {
    Hash index = get_index(hash);
    clear_entry_if_different_hash(table, hash, index);
    table[index].heuristic_value = (short) heuristic_value;
    table[index].key = hash;
}


void add_forced_move_to_table(TableEntry* table, Hash hash, Coord forced_move) {
    Hash index = get_index(hash);
    clear_entry_if_different_hash(table, hash, index);
    table[index].forced_move = forced_move;
    table[index].key = hash;
}


void add_book_move_to_table(TableEntry* table, Hash hash, int8_t x, int8_t y) {
    Coord book_move = {x, y};
    Hash index = get_index(hash);
    table[index].book_move = book_move;
    table[index].key = hash;
}