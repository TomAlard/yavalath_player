#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "random.h"
#include "board.h"
#include "util.h"
#include "line.h"
#include "heuristics.h"
#include "transposition_table.h"


typedef struct Board {
    int8_t board[BOARD_SIZE][BOARD_SIZE];
    Hash hash_keys[BOARD_SIZE][BOARD_SIZE][2];
    Hash hash;
    Heuristics* heuristics;
    TableEntry* transposition_table;
} Board;


void set_id(Board* board, Coord coord, int8_t id) {
    board->board[coord.x][coord.y] = id;
}


void init_board_board(Board* board) {
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            Coord c = {x, y};
            int8_t id = coord_is_valid(c)? 0 : -1;
            set_id(board, c, id);
        }
    }
}


void init_board_hash_keys(Board* board) {
    pcg32_random_t rng;
    pcg32_srandom_r(&rng, 69, 420);
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int player = 0; player < 2; player++) {
                board->hash_keys[x][y][player] = ((uint64_t)pcg32_random_r(&rng) << 32) | pcg32_random_r(&rng);
            }
        }
    }
}


Board* init_board() {
    Board* board = safe_malloc(sizeof(Board));
    board->transposition_table = create_transposition_table();
    board->heuristics = create_heuristics();
    init_board_board(board);
    init_board_hash_keys(board);
    board->hash = 0;
    return board;
}


void free_board(Board* board) {
    free(board->transposition_table);
    free(board->heuristics);
    free(board);
}


int init_position(Board* board, char* filename) {
    char* s = "../";
    char* result = safe_malloc(strlen(s) + strlen(filename) + 1);
    strcpy(result, s);
    strcat(result, filename);
    FILE* file = safe_fopen(result, "r");

    free(result);
    uint8_t current_id = 1;
    int c = 0;
    while (c != EOF) {
        int y = fgetc(file) - '0';
        if (y + '0' == EOF) {
            break;
        }
        int x = fgetc(file) - '0';
        c = fgetc(file);
        Coord coord = {x, y};
        make_move(board, coord, current_id);
        current_id = opposite_id(current_id);
    }
    return current_id;
}


int8_t get_id(Board* board, Coord coord) {
    return board->board[coord.x][coord.y];
}


void make_move(Board* board, Coord coord, uint8_t current_id) {
    my_assert(current_id != 0, "make_move: current id is 0!");
    board->hash ^= board->hash_keys[coord.x][coord.y][current_id - 1];
    set_id(board, coord, current_id);
}


void undo_move(Board* board, Coord coord) {
    uint8_t id = get_id(board, coord);
    my_assert(id != 0, "undo_move: undoing move with id 0!");
    board->hash ^= board->hash_keys[coord.x][coord.y][id - 1];
    set_id(board, coord, 0);
}


Board* dont_use_this_board;
uint8_t dont_use_this_current_id;
int dont_use_this_current_heuristic_value;
int calculate_heuristic(Coord move) {
    make_move(dont_use_this_board, move, dont_use_this_current_id);
    int heuristic = get_heuristic_value_of_board(dont_use_this_board, move, dont_use_this_current_heuristic_value);
    undo_move(dont_use_this_board, move);
    return heuristic;
}

int comparator(const void* a, const void* b) {
    return calculate_heuristic(*(Coord*) b) - calculate_heuristic(*(Coord*) a);
}

int iterate(Board* board, Coord moves[BOARD_SQUARES], uint8_t current_id, int heuristic_value, bool sort_moves) {
    int amount_of_moves = 0;
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            Coord c = {x, y};
            if (get_id(board, c) == 0) {
                set_coord(&moves[amount_of_moves++], c);
            }
        }
    }
    if (sort_moves) {
        dont_use_this_board = board;
        dont_use_this_current_id = current_id;
        dont_use_this_current_heuristic_value = heuristic_value;
        qsort(moves, amount_of_moves, sizeof(Coord), comparator);
    }
    return amount_of_moves;
}


// From the opponent's perspective
int score_board(Board* board, Coord last_move) {
    bool line3_found = false;
    for (int direction = 0; direction < 3; direction++) {
        int line_length = calculate_line_length(board, last_move, direction)
                          + calculate_line_length(board, last_move, opposite_direction(direction)) - 1;
        if (line_length == 4) {
            return LOSS;
        }
        if (line_length == 3) {
            line3_found = true;
        }
    }
    return line3_found? WIN : 0;
}


int get_true_value_of_board(Board* board, Coord last_move) {
    my_assert(get_id(board, last_move) != 0, "get_true_value_of_board: id is 0!");
    int table_true_value = get_true_value_from_table(board->transposition_table, board->hash);
    if (true_value_is_valid(table_true_value)) {
        return table_true_value;
    }

    int true_value = score_board(board, last_move);
    add_true_value_to_table(board->transposition_table, board->hash, true_value);
    return true_value;
}


int get_heuristic_value_of_board(Board* board, Coord last_move, int current_heuristic_value) {
    int table_heuristic_value = get_heuristic_value_from_table(board->transposition_table, board->hash);
    if (heuristic_value_is_valid(table_heuristic_value)) {
        return table_heuristic_value;
    }

    Coord forced_move = INVALID_COORD;
    uint8_t current_id = get_id(board, last_move);
    int added_heuristic_value = heuristic_analysis(board, board->heuristics, last_move, current_id, &forced_move);
    int heuristic_value = added_heuristic_value == 1000
            ? added_heuristic_value
            : current_heuristic_value + added_heuristic_value;

    add_heuristic_value_to_table(board->transposition_table, board->hash, heuristic_value);
    my_assert(get_id(board, forced_move) == 0, "get_heuristic_value_of_board: Forced move has non zero id!");
    add_forced_move_to_table(board->transposition_table, board->hash, forced_move);
    return heuristic_value;
}


Coord get_forced_move_in_position(Board* board) {
    Coord move = get_forced_move_from_table(board->transposition_table, board->hash);
    my_assert(get_id(board, move) == 0, "get_forced_move_in_position: Forced move has non zero id!");
    return move;
}


int get_position_heuristic_of_move(Board* board, Coord move) {
    return get_position_heuristic(board->heuristics, move);
}


void play_move(Board* board, uint8_t my_id, Coord move, char* message, bool print) {
    make_move(board, move, my_id);
    if (print) {
        printf("%d %d %s\n", move.y, move.x, message);
    }
}
