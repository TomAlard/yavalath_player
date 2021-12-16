#include "minimax.h"
#include "util.h"


typedef struct State {
    Coord last_move;
    uint8_t current_id;
    int current_depth;
    int max_depth;
    int alpha;
    int beta;
    int heuristic_value;
} State;


int try_move(Game* game, uint8_t current_id, Coord current_move, int heuristic_value) {
    make_move(game, current_move, current_id);
    return get_heuristic_value_of_game(game, current_move, heuristic_value);
}


int evaluate_move(Game* game, State state, Coord forced_move, clock_t deadline, Coord* best_move);


State build_next_state(State state, Coord last_move, int heuristic_value) {
    state.last_move = last_move;
    state.heuristic_value = heuristic_value;
    return state;
}


int alphabeta(Game* game, State state, clock_t deadline, Coord* best_move) {
    if (!has_time_remaining(deadline)) {
        return 0;
    }
    bool is_max_depth = state.current_depth == state.max_depth;
    int true_value = get_true_value_of_game(game, state.last_move);
    if (true_value != 0 && !is_max_depth) {
        return true_value;
    }
    if (state.current_depth == 0) {
        return state.heuristic_value;
    }

    int previous_heuristic_value = state.heuristic_value;
    my_assert(previous_heuristic_value != WIN && previous_heuristic_value != LOSS,
              "Alphabeta previous heuristic value is WIN/LOSS");
    int best_minimax_value = -INF;
    Coord moves[BOARD_SQUARES];

    int amount_of_moves = iterate(game, moves, state.current_id, previous_heuristic_value, is_max_depth);
    for (int i = 0; i < amount_of_moves; i++) {
        Coord move = moves[i];
        int move_heuristic_value = try_move(game, state.current_id, move, previous_heuristic_value);
        Coord forced_move = get_forced_move_in_position(game);
        my_assert(forced_move.x != move.x || forced_move.y != move.y, "Alphabeta: forced move is move!");
        State next_state = build_next_state(state, move, move_heuristic_value);
        int current_move_minimax_value = evaluate_move(game, next_state, forced_move, deadline, best_move);
        undo_move(game, move);
        if (current_move_minimax_value > best_minimax_value) {
            best_minimax_value = current_move_minimax_value;
            state.heuristic_value = move_heuristic_value;
            state.last_move = move;
            if (is_max_depth) {
                set_coord(best_move, move);
            }
        }
        state.alpha = max_(state.alpha, current_move_minimax_value);
        if (!has_time_remaining(deadline)) {
            return 0;
        }
        if (state.alpha >= state.beta) {
            break;
        }
    }
    add_minimax_value_of_position(game, best_minimax_value);
    return best_minimax_value;
}


int make_forced_move(Game* game, State state, Coord forced_move, clock_t deadline, Coord* best_move) {
    state.current_depth++;
    int true_value = get_true_value_of_game(game, state.last_move);
    if (true_value != 0 || !has_time_remaining(deadline)) {
        return true_value;
    }

    state.heuristic_value = try_move(game, state.current_id, forced_move, state.heuristic_value);
    state.last_move = forced_move;
    Coord next_forced_move = get_forced_move_in_position(game);
    int forced_move_minimax_value = evaluate_move(game, state, next_forced_move, deadline, best_move);
    undo_move(game, forced_move);
    return forced_move_minimax_value;
}


int evaluate_move(Game* game, State state, Coord forced_move, clock_t deadline, Coord* best_move) {
    state.current_id = opposite_id(state.current_id);
    int tmp = state.alpha;
    state.alpha = -state.beta;
    state.beta = -tmp;
    state.current_depth--;
    state.heuristic_value *= -1;
    return coord_is_valid(forced_move)
           ? -make_forced_move(game, state, forced_move, deadline, best_move)
           : -alphabeta(game, state, deadline, best_move);
}


int calculate_minimax_value(Game* game, Coord last_move, int max_depth, clock_t deadline, int heuristic_value,
                            Coord* best_move) {
    uint8_t current_id = opposite_id(get_id_of_move(game, last_move));
    State state = {last_move, current_id, max_depth, max_depth, LOSS, WIN, heuristic_value};
    return alphabeta(game, state, deadline, best_move);
}
