#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "random.h"
#include "game.h"
#include "util.h"
#include "line.h"
#include "heuristics.h"
#include "transposition_table.h"


typedef struct Game {
    int8_t game[BOARD_SIZE][BOARD_SIZE];
    Hash hash_keys[BOARD_SIZE][BOARD_SIZE][2];
    Hash hash;
    Heuristics* heuristics;
    TableEntry* transposition_table;
} Game;


void set_id(Game* game, Coord coord, int8_t id) {
    game->game[coord.x][coord.y] = id;
}


void init_game_game(Game* game) {
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            Coord c = {x, y};
            int8_t id = coord_is_valid(c)? 0 : -1;
            set_id(game, c, id);
        }
    }
}


void init_game_hash_keys(Game* game) {
    pcg32_random_t rng;
    pcg32_srandom_r(&rng, 69, 420);
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int player = 0; player < 2; player++) {
                game->hash_keys[x][y][player] = ((uint64_t)pcg32_random_r(&rng) << 32) | pcg32_random_r(&rng);
            }
        }
    }
}


Game* init_game() {
    Game* game = safe_malloc(sizeof(Game));
    game->transposition_table = create_transposition_table();
    game->heuristics = create_heuristics();
    init_game_game(game);
    init_game_hash_keys(game);
    game->hash = 0;
    return game;
}


void free_game(Game* game) {
    free(game->transposition_table);
    free(game->heuristics);
    free(game);
}


int init_position(Game* game, char* filename) {
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
        make_move(game, coord, current_id);
        current_id = opposite_id(current_id);
    }
    return current_id;
}


int8_t get_id(Game* game, Coord coord) {
    return game->game[coord.x][coord.y];
}


void make_move(Game* game, Coord coord, uint8_t current_id) {
    my_assert(current_id != 0, "make_move: current id is 0!");
    game->hash ^= game->hash_keys[coord.x][coord.y][current_id - 1];
    set_id(game, coord, current_id);
}


void undo_move(Game* game, Coord coord) {
    uint8_t id = get_id(game, coord);
    my_assert(id != 0, "undo_move: undoing move with id 0!");
    game->hash ^= game->hash_keys[coord.x][coord.y][id - 1];
    set_id(game, coord, 0);
}


Game* dont_use_this_game;
uint8_t dont_use_this_current_id;
int dont_use_this_current_heuristic_value;
int calculate_heuristic(Coord move) {
    make_move(dont_use_this_game, move, dont_use_this_current_id);
    int heuristic = get_heuristic_value_of_game(dont_use_this_game, move, dont_use_this_current_heuristic_value);
    undo_move(dont_use_this_game, move);
    return heuristic;
}

int comparator(const void* a, const void* b) {
    return calculate_heuristic(*(Coord*) b) - calculate_heuristic(*(Coord*) a);
}

int iterate(Game* game, Coord moves[BOARD_SQUARES], uint8_t current_id, int heuristic_value, bool sort_moves) {
    int amount_of_moves = 0;
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            Coord c = {x, y};
            if (get_id(game, c) == 0) {
                set_coord(&moves[amount_of_moves++], c);
            }
        }
    }
    if (sort_moves) {
        dont_use_this_game = game;
        dont_use_this_current_id = current_id;
        dont_use_this_current_heuristic_value = heuristic_value;
        qsort(moves, amount_of_moves, sizeof(Coord), comparator);
    }
    return amount_of_moves;
}


// From the opponent's perspective
int score_game(Game* game, Coord last_move) {
    bool line3_found = false;
    for (int direction = 0; direction < 3; direction++) {
        int line_length = calculate_line_length(game, last_move, direction)
                          + calculate_line_length(game, last_move, opposite_direction(direction)) - 1;
        if (line_length == 4) {
            return LOSS;
        }
        if (line_length == 3) {
            line3_found = true;
        }
    }
    return line3_found? WIN : 0;
}


int get_true_value_of_game(Game* game, Coord last_move) {
    my_assert(get_id(game, last_move) != 0, "get_true_value_of_game: id is 0!");
    int table_true_value = get_true_value_from_table(game->transposition_table, game->hash);
    if (true_value_is_valid(table_true_value)) {
        return table_true_value;
    }

    int true_value = score_game(game, last_move);
    add_true_value_to_table(game->transposition_table, game->hash, true_value);
    return true_value;
}


int get_heuristic_value_of_game(Game* game, Coord last_move, int current_heuristic_value) {
    int table_heuristic_value = get_heuristic_value_from_table(game->transposition_table, game->hash);
    if (heuristic_value_is_valid(table_heuristic_value)) {
        return table_heuristic_value;
    }

    Coord forced_move = INVALID_COORD;
    uint8_t current_id = get_id(game, last_move);
    int added_heuristic_value = heuristic_analysis(game, game->heuristics, last_move, current_id, &forced_move);
    int heuristic_value = added_heuristic_value == 1000
            ? added_heuristic_value
            : current_heuristic_value + added_heuristic_value;

    add_heuristic_value_to_table(game->transposition_table, game->hash, heuristic_value);
    my_assert(get_id(game, forced_move) == 0, "get_heuristic_value_of_game: Forced move has non zero id!");
    add_forced_move_to_table(game->transposition_table, game->hash, forced_move);
    return heuristic_value;
}


Coord get_forced_move_in_position(Game* game) {
    Coord move = get_forced_move_from_table(game->transposition_table, game->hash);
    my_assert(get_id(game, move) == 0, "get_forced_move_in_position: Forced move has non zero id!");
    return move;
}


int get_position_heuristic_of_move(Game* game, Coord move) {
    return get_position_heuristic(game->heuristics, move);
}


void play_move(Game* game, uint8_t my_id, Coord move, char* message, bool print) {
    make_move(game, move, my_id);
    if (print) {
        printf("%d %d %s\n", move.y, move.x, message);
    }
}