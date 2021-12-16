#include <stdbool.h>
#include <stdio.h>
#include "game.h"
#include "util.h"
#include "minimax.h"
#include "../test/test_main.h"


bool handle_opening(Game* game, uint8_t my_id, Coord enemy_move, int* heuristic_value, bool print) {
    if (!coord_is_valid(enemy_move)) {
        Coord move = {2, 2};
        play_move(game, my_id, move, "BOOK", print);
        *heuristic_value = -2;
        return true;
    }
    if (get_position_heuristic_of_move(game, enemy_move) >= 2) {
        make_move(game, enemy_move, opposite_id(my_id));
        play_move(game, my_id, enemy_move, "BOOK", print);
        *heuristic_value = -get_position_heuristic_of_move(game, enemy_move);
        return true;
    }
    return false;
}


bool handle_forced_move(Game* game, uint8_t my_id, int* heuristic_value, bool print) {
    Coord forced_move = get_forced_move_in_position(game);
    bool is_forced_move = coord_is_valid(forced_move);
    if (is_forced_move) {
        play_move(game, my_id, forced_move, "FORCED", print);
        *heuristic_value = get_heuristic_value_of_game(game, forced_move, *heuristic_value);
    }
    return is_forced_move;
}


bool handle_instant_win(Game* game, uint8_t my_id, int* heuristic_value, bool print) {
    Coord moves[BOARD_SQUARES];
    int amount_of_moves = iterate(game, moves, my_id, *heuristic_value, false);
    for (int i = 0; i < amount_of_moves; i++) {
        Coord move = moves[i];
        make_move(game, move, my_id);
        int true_value = -get_true_value_of_game(game, move);
        undo_move(game, move);
        if (true_value == WIN) {
            play_move(game, my_id, move, "INSTANT WIN", print);
            *heuristic_value = WIN;
            return true;
        }
    }
    return false;
}


int play_best_move(Game* game, uint8_t my_id, Coord enemy_move, bool first, int previous_heuristic_value, double time,
                   bool print) {
    int heuristic_value = previous_heuristic_value;
    if (first && handle_opening(game, my_id, enemy_move, &heuristic_value, print)) {
        return heuristic_value;
    }
    make_move(game, enemy_move, opposite_id(my_id));
    Coord book_move = get_book_move_in_position(game);
    if (coord_is_valid(book_move)) {
        play_move(game, my_id, book_move, "BOOK", print);
        return -get_position_heuristic_of_move(game, enemy_move);
    }
    heuristic_value = -get_heuristic_value_of_game(game, enemy_move, previous_heuristic_value);
    if (handle_instant_win(game, my_id, &heuristic_value, print) || handle_forced_move(game, my_id, &heuristic_value, print)) {
        return heuristic_value;
    }

    int value = -INF;
    Coord result = INVALID_COORD;
    int max_depth = 0;
    clock_t deadline = get_deadline(time);
    while (has_time_remaining(deadline) && value != WIN && value != LOSS) {
        max_depth++;
        Coord new_best_move = INVALID_COORD;
        int new_value = calculate_minimax_value(game, enemy_move, max_depth, deadline, heuristic_value, &new_best_move);
        if (has_time_remaining(deadline)) {
            if (print) fprintf(stderr, "value: '%d', result: '(%d, %d)'\n", new_value, new_best_move.y, new_best_move.x);
            value = new_value;
            result = new_best_move;
        }
    }
    if (print) fprintf(stderr, "Depth: %d\n", max_depth-1);

    char* msg;
    if (value == WIN) msg = "WIN";
    else if (value == LOSS) msg = "LOSS";
    else {
        char eval_msg[10];
        sprintf(eval_msg, "%d", value);
        msg = eval_msg;
    }
    play_move(game, my_id, result, msg, print);
    return get_heuristic_value_of_game(game, result, heuristic_value);
}


#define TIME 5
int main() {
    // run_tests();
    int my_id;
    scanf("%d", &my_id);
    Game* game = init_game();
    int heuristic_value = 0;
    bool first = true;
    while (true) {
        int count;
        int amount_read = scanf("%d", &count);
        if (amount_read == 0 || amount_read == EOF) {
            break;
        }
        for (int i = 0; i < count; i++) {
            char line[12];
            scanf("%s", line);
        }
        int enemy_y;
        int enemy_x;
        scanf("%d%d", &enemy_y, &enemy_x);
        Coord enemy_move = {enemy_x, enemy_y};
        heuristic_value = play_best_move(game, my_id, enemy_move, first, heuristic_value, TIME, true);
        fflush(stdout);
        first = false;
    }
}
