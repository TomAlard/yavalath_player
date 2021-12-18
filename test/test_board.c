#include <stdio.h>
#include <stdlib.h>
#include "test_board.h"
#include "../src/board.h"
#include "../src/util.h"


void test_init_board_all_ids_zero(Board* board) {
    char* fail_msg = "test_init_board_all_ids_zero: Not all ids are zero";
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            Coord c = {x, y};
            if (coord_is_valid(c)) {
                my_assert(get_id(board, c) == 0, fail_msg);
            }
        }
    }
}


void test_coord_to_index() {
    char* fail_msg = "test_coord_to_index: Incorrect conversion";
    int expected = BOARD_SQUARES - 1;
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            Coord c = {x, y};
            if (coord_is_valid(c)) {
                int result = coord_to_index(c);
                my_assert(result == expected--, fail_msg);
            }
        }
    }
}


void test_set_get_id(Board* board) {
    char* fail_msg = "test_set_get_id: set_id Incorrectly changes id or get_id incorrectly retrieves the id";
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            Coord c = {x, y};
            if (coord_is_valid(c)) {
                set_id(board, c, 1);
                int id = get_id(board, c);
                my_assert(id == 1, fail_msg);
                set_id(board, c, 2);
                id = get_id(board, c);
                my_assert(id == 2, fail_msg);
                set_id(board, c, 0);
                id = get_id(board, c);
                my_assert(id == 0, fail_msg);
            }
        }
    }
}


void validate_true_values(Board* board, const Coord* moves, const int* expected_true_values, char* fail_msg) {
    for (int id = 1; id <= 2; id++) {
        for (int i = 0; i < 4; i++) {
            Coord move = moves[i];
            set_id(board, move, id);
            int true_value = calculate_true_value(board, move);
            my_assert(true_value == expected_true_values[i], fail_msg);
        }

        for (int i = 0; i < 4; i++) {
            set_id(board, moves[i], 0);
        }
    }
}


void test_calculate_true_value(Board* board) {
    char* fail_msg = "test_calculate_true_value: Incorrect true value";
    int expected_true_values[] = {UNDECIDED, UNDECIDED, LOSS, WIN};

    Coord moves_horizontal[] = {{0, 0}, {0, 1}, {0, 2}, {0, 3}};
    validate_true_values(board, moves_horizontal, expected_true_values, fail_msg);

    Coord moves_down_right[] = {{4, 5}, {5, 5}, {6, 5}, {7, 5}};
    validate_true_values(board, moves_down_right, expected_true_values, fail_msg);

    Coord moves_down_left[] = {{2, 3}, {3, 3}, {4, 3}, {5, 2}};
    validate_true_values(board, moves_down_left, expected_true_values, fail_msg);
}


void validate_heuristic_values(Board* board, const Coord* moves, const int* expected_sign, char* fail_msg) {
    for (int id = 1; id <= 2; id++) {
        for (int i = 0; i < 3; i++) {
            Coord move = moves[i];
            set_id(board, move, id);
            int heuristic_value = calculate_heuristic_value(board, move);
            my_assert(sign(heuristic_value) == expected_sign[i], fail_msg);
        }
        set_id(board, moves[1], 0);
        int heuristic_value2 = calculate_heuristic_value(board, moves[2]);
        my_assert(sign(heuristic_value2) == expected_sign[3], fail_msg);
        for (int i = 0; i < 4; i++) {
            set_id(board, moves[i], 0);
        }
    }
}


void test_calculate_heuristic_value(Board* board) {
    char* fail_msg = "test_calculate_heuristic_value: Incorrect heuristic value";
    int expected_sign[] = {0, 1, 1, -1};

    Coord moves_horizontal[] = {{0, 0}, {0, 3}, {0, 2}};
    validate_heuristic_values(board, moves_horizontal, expected_sign, fail_msg);

    Coord moves_down_right[] = {{4, 5}, {7, 5}, {6, 5}};
    validate_heuristic_values(board, moves_down_right, expected_sign, fail_msg);

    Coord moves_down_left[] = {{2, 3}, {5, 2}, {4, 3}};
    validate_heuristic_values(board, moves_down_left, expected_sign, fail_msg);
}


void run_board_tests() {
    Board* board = init_board();
    fprintf(stderr, "test_init_board_all_ids_zero\n");
    test_init_board_all_ids_zero(board);
    fprintf(stderr, "test_coord_to_index\n");
    test_coord_to_index();
    fprintf(stderr, "test_set_get_id\n");
    test_set_get_id(board);
    fprintf(stderr, "test_calculate_true_value\n");
    test_calculate_true_value(board);
    fprintf(stderr, "test_calculate_heuristic_value\n");
    test_calculate_heuristic_value(board);
    free(board);
}