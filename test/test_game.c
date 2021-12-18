#include "test_game.h"
#include "../src/util.h"
#include "../src/game.h"


void test_init_game() {
    Game* game = init_game();
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            Coord coord = {x, y};
            if (coord_is_valid(coord)) {
                my_assert(get_id_of_move(game, coord) == 0,
                          "test_init_game: Incorrectly initialized id's");
            }
        }
    }
    free_game(game);
}


int get_move_value(Game* game, Coord moves[BOARD_SQUARES], int index, uint8_t id, int heuristic_value) {
    Coord move = moves[index];
    make_move(game, move, id);
    int result = get_heuristic_value_of_game(game, move, heuristic_value);
    undo_move(game, move);
    return result;
}


void test_iterate() {
    Game* game = init_game();
    int id = init_position(game, "test_positions/instant_win.txt");
    Coord moves[BOARD_SQUARES];
    for (int heuristic_value = -10; heuristic_value <= 10; heuristic_value += 5) {
        int amount_of_moves = iterate(game, moves, id, heuristic_value, true);
        int best_value = get_move_value(game, moves, 0, id, heuristic_value);
        int worst_value = get_move_value(game, moves, amount_of_moves-1, id, heuristic_value);
        int middle_value = get_move_value(game, moves, amount_of_moves/2, id, heuristic_value);
        my_assert(best_value >= middle_value && middle_value >= worst_value, "test_iterate: Incorrect sorting");
    }
    free_game(game);
}


void test_get_true_value_of_game() {
    Game* game = init_game();
    int id = init_position(game, "test_positions/instant_win.txt");
    char* fail_msg = "test_get_true_value_of_game: Incorrect true value";
    int true_value;

    Coord last_move = {0, 3};
    true_value = get_true_value_of_game(game, last_move);
    my_assert(true_value == 0, fail_msg);

    Coord winning_move = {4, 6};
    make_move(game, winning_move, id);
    true_value = get_true_value_of_game(game, winning_move);
    undo_move(game, winning_move);
    my_assert(true_value == LOSS, fail_msg);

    Coord losing_move = {4, 3};
    make_move(game, losing_move, id);
    true_value = get_true_value_of_game(game, losing_move);
    undo_move(game, losing_move);
    my_assert(true_value == WIN, fail_msg);

    free_game(game);
}


void test_get_heuristic_value_of_game() {
    Game* game = init_game();
    int id = init_position(game, "test_positions/good_for_player_1.txt");
    Coord good_move = {5, 5};
    make_move(game, good_move, id);
    int value = get_heuristic_value_of_game(game, good_move, 0);
    char* fail_msg = "test_get_heuristic_value_of_game: Heuristic value should be 42";
    my_assert(value == 42, fail_msg);
    free_game(game);

    game = init_game();
    id = init_position(game, "test_positions/double_forced_move.txt");
    Coord forces_two_moves = {4, 4};
    make_move(game, forces_two_moves, id);
    value = get_heuristic_value_of_game(game, forces_two_moves, 0);
    char* fail_msg2 = "test_get_heuristic_value_of_game: Heuristic value should be 1000";
    my_assert(value > DOUBLE_FORCING - 50 && value < DOUBLE_FORCING + 50, fail_msg2);
    free_game(game);
}


void test_get_id() {
    Game* game = init_game();
    char* fail_msg = "test_get_id: Got wrong ID";
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            Coord move = {x, y};
            if (coord_is_valid(move)) {
                for (int id = 1; id <= 2; id++) {
                    make_move(game, move, id);
                    my_assert(get_id_of_move(game, move) == id, fail_msg);
                    undo_move(game, move);
                }
            }
        }
    }
    free_game(game);
}


void test_get_forced_move_in_position() {
    Game* game = init_game();
    int id = init_position(game, "test_positions/instant_win.txt");
    Coord forcing_move = {4, 2};
    make_move(game, forcing_move, id);
    get_heuristic_value_of_game(game, forcing_move, 0);  // puts forced move in transposition table
    Coord forced_move = get_forced_move_in_position(game);
    char* fail_msg = "test_get_forced_move_in_position: Got incorrect forced move";
    my_assert(coord_equals_xy(forced_move, 4, 3), fail_msg);
    free_game(game);
}


void run_game_tests() {
    fprintf(stderr, "test_init_game\n");
    test_init_game();
    fprintf(stderr, "test_iterate\n");
    test_iterate();
    fprintf(stderr, "test_get_true_value_of_game\n");
    test_get_true_value_of_game();
    fprintf(stderr, "test_get_heuristic_value_of_game\n");
    test_get_heuristic_value_of_game();
    fprintf(stderr, "test_get_id\n");
    test_get_id();
    fprintf(stderr, "test_get_forced_move_in_position\n");
    test_get_forced_move_in_position();
}