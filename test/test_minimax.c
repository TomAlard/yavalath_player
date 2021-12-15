#include <time.h>
#include "test_minimax.h"
#include "../src/minimax.h"
#include "../src/util.h"
#include "../src/main.h"


uint8_t play_game(Game* game, Coord first_move) {
    int true_value = 0;
    uint8_t current_id = opposite_id(get_id_of_move(game, first_move));
    Coord last_move = first_move;
    while (true_value == 0) {
        play_best_move(game, current_id, last_move, false, 0, INF, false);
        current_id = opposite_id(current_id);
        true_value = get_true_value_of_game(game, last_move);
    }
    return true_value == WIN? current_id : opposite_id(current_id);
}


void test_finds_win() {
    Game* game = init_game();
    int id = init_position(game, "test_positions/good_for_player_1.txt");
    clock_t deadline = get_deadline(INF);
    Coord move = {5, 5};
    make_move(game, move, id);
    Coord bad_response = {8, 4};
    make_move(game, bad_response, opposite_id(id));
    Coord best_move = INVALID_COORD;
    int value = calculate_minimax_value(game, bad_response, 4, deadline, 0, &best_move);
    char* fail_msg = "test_finds_win: Incorrect continuation";
    my_assert(coord_equals_xy(best_move, 4, 3) || coord_equals_xy(best_move, 3, 3)
                || coord_equals_xy(best_move, 3, 4) || coord_equals_xy(best_move, 4, 5)
                || coord_equals_xy(best_move, 5, 4) || coord_equals_xy(best_move, 5, 3), fail_msg);
    char* fail_msg2 = "test_finds_win: Minimax value should be WIN";
    my_assert(value == WIN, fail_msg2);
    char* fail_msg3 = "test_finds_win: Player 1 should win";
    my_assert(play_game(game, best_move) == 1, fail_msg3);
    free_game(game);
}


void run_minimax_tests() {
    fprintf(stderr, "test_finds_win\n");
    test_finds_win();
}