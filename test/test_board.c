#include "test_board.h"
#include "../src/util.h"
#include "../src/board.h"


void test_init_board() {
    Board* board = init_board();
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            Coord coord = {x, y};
            int expected_id = coord_is_valid(coord)? 0 : -1;
            my_assert(get_id(board, coord) == expected_id, "test_init_board: Incorrectly initialized id's");
        }
    }
    free_board(board);
}


int get_move_value(Board* board, Coord moves[BOARD_SQUARES], int index, uint8_t id, int heuristic_value) {
    Coord move = moves[index];
    make_move(board, move, id);
    int result = get_heuristic_value_of_board(board, move, heuristic_value);
    undo_move(board, move);
    return result;
}


void test_iterate() {
    Board* board = init_board();
    int id = init_position(board, "test_positions/instant_win.txt");
    Coord moves[BOARD_SQUARES];
    for (int heuristic_value = -10; heuristic_value <= 10; heuristic_value += 5) {
        int amount_of_moves = iterate(board, moves, id, heuristic_value, true);
        int best_value = get_move_value(board, moves, 0, id, heuristic_value);
        int worst_value = get_move_value(board, moves, amount_of_moves-1, id, heuristic_value);
        int middle_value = get_move_value(board, moves, amount_of_moves/2, id, heuristic_value);
        my_assert(best_value >= middle_value && middle_value >= worst_value, "test_iterate: Incorrect sorting");
    }
    free_board(board);
}


void test_get_true_value_of_board() {
    Board* board = init_board();
    int id = init_position(board, "test_positions/instant_win.txt");
    char* fail_msg = "test_get_true_value_of_board: Incorrect true value";
    int true_value;

    Coord last_move = {0, 3};
    true_value = get_true_value_of_board(board, last_move);
    my_assert(true_value == 0, fail_msg);

    Coord winning_move = {4, 6};
    make_move(board, winning_move, id);
    true_value = get_true_value_of_board(board, winning_move);
    undo_move(board, winning_move);
    my_assert(true_value == LOSS, fail_msg);

    Coord losing_move = {4, 3};
    make_move(board, losing_move, id);
    true_value = get_true_value_of_board(board, losing_move);
    undo_move(board, losing_move);
    my_assert(true_value == WIN, fail_msg);

    free_board(board);
}


void test_get_heuristic_value_of_board() {
    Board* board = init_board();
    int id = init_position(board, "test_positions/good_for_player_1.txt");
    Coord good_move = {5, 5};
    make_move(board, good_move, id);
    int value = get_heuristic_value_of_board(board, good_move, 0);
    char* fail_msg = "test_get_heuristic_value_of_board: Heuristic value should be 42";
    my_assert(value == 42, fail_msg);
    free_board(board);

    board = init_board();
    id = init_position(board, "test_positions/double_forced_move.txt");
    Coord forces_two_moves = {4, 4};
    make_move(board, forces_two_moves, id);
    value = get_heuristic_value_of_board(board, forces_two_moves, 0);
    char* fail_msg2 = "test_get_heuristic_value_of_board: Heuristic value should be 1000";
    my_assert(value == 1000, fail_msg2);
    free_board(board);
}


void test_get_id() {
    Board* board = init_board();
    char* fail_msg = "test_get_id: Got wrong ID";
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            Coord move = {x, y};
            if (coord_is_valid(move)) {
                for (int id = 1; id <= 2; id++) {
                    make_move(board, move, id);
                    my_assert(get_id(board, move) == id, fail_msg);
                    undo_move(board, move);
                }
            }
        }
    }
    free_board(board);
}


void test_get_forced_move_in_position() {
    Board* board = init_board();
    int id = init_position(board, "test_positions/instant_win.txt");
    Coord forcing_move = {4, 2};
    make_move(board, forcing_move, id);
    get_heuristic_value_of_board(board, forcing_move, 0);  // puts forced move in transposition table
    Coord forced_move = get_forced_move_in_position(board);
    char* fail_msg = "test_get_forced_move_in_position: Got incorrect forced move";
    my_assert(coord_equals_xy(forced_move, 4, 3), fail_msg);
    free_board(board);
}


void run_board_tests() {
    fprintf(stderr, "test_init_board\n");
    test_init_board();
    fprintf(stderr, "test_iterate\n");
    test_iterate();
    fprintf(stderr, "test_get_true_value_of_board\n");
    test_get_true_value_of_board();
    fprintf(stderr, "test_get_heuristic_value_of_board\n");
    test_get_heuristic_value_of_board();
    fprintf(stderr, "test_get_id\n");
    test_get_id();
    fprintf(stderr, "test_get_forced_move_in_position\n");
    test_get_forced_move_in_position();
}