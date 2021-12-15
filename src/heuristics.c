#include <string.h>
#include "heuristics.h"
#include "util.h"


typedef struct Heuristics {
    int position_heuristic[BOARD_SIZE][BOARD_SIZE];
    int patterns_heuristic[81];
} Heuristics;


void set_position_heuristic(Heuristics* heuristic) {
    int position_heuristic[BOARD_SIZE][BOARD_SIZE] = {
            {0, 0, 0, 0, 0, -1, -1, -1, -1},
            {0, 1, 1, 1, 1, 0, -1, -1, -1},
            {0, 1, 2, 2, 2, 1, 0, -1, -1},
            {0, 1, 2, 3, 3, 2, 1, 0, -1},
            {0, 1, 2, 3, 4, 3, 2, 1, 0},
            {0, 1, 2, 3, 3, 2, 1, 0, -1},
            {0, 1, 2, 2, 2, 1, 0, -1, -1},
            {0, 1, 1, 1, 1, 0, -1, -1, -1},
            {0, 0, 0, 0, 0, -1, -1, -1, -1}
    };
    memcpy(heuristic->position_heuristic, position_heuristic, sizeof(heuristic->position_heuristic));
}


#define FORCED_MOVE_VALUE 70
#define GOOD 10
#define BAD -GOOD
// From Player 1's perspective
void set_patterns_heuristic(Heuristics* heuristic) {
    int h[81] = {0};
    h[40] = WIN;  // 1111
    h[80] = LOSS;  // 2222
    h[39] = LOSS;  // 1110
    h[78] = WIN;  // 2220
    h[41] = LOSS;  // 1112
    h[79] = WIN;  // 2221
    h[37] = FORCED_MOVE_VALUE;  // 1101
    h[74] = -FORCED_MOVE_VALUE;  // 2202
    h[31] = FORCED_MOVE_VALUE;  // 1011
    h[62] = -FORCED_MOVE_VALUE;  // 2022
    h[28] = 2*GOOD;  // 1001
    h[56] = 2*BAD;  // 2002
    h[10] = BAD;  // 0101
    h[20] = GOOD;  // 0202
    h[30] = BAD;  // 1010
    h[60] = GOOD;  // 2020
    h[12] = BAD;  // 0110
    h[24] = GOOD;  // 0220
    h[36] = BAD;  // 1100
    h[72] = GOOD;  // 2200
    h[4] = BAD;  // 0011
    h[8] = GOOD;  // 0022
    memcpy(heuristic->patterns_heuristic, h, sizeof(heuristic->patterns_heuristic));
}


Heuristics* create_heuristics() {
    Heuristics* heuristics = safe_malloc(sizeof(Heuristics));
    set_position_heuristic(heuristics);
    set_patterns_heuristic(heuristics);
    return heuristics;
}


int get_pattern_value(Heuristics* heuristics, const int8_t pattern[7], int start, uint8_t current_id) {
    if (pattern[start] == -1 || pattern[start+3] == -1) {
        return 0;
    }
    int index = 27*pattern[start] + 9*pattern[start+1] + 3*pattern[start+2] + pattern[start+3];
    return heuristics->patterns_heuristic[index] * (current_id == 1? 1 : -1);
}


int find_pattern_start(const int8_t pattern[7]) {
    int line_start = 0;
    while (pattern[line_start] == -1) {
        line_start++;
    }
    return line_start;
}


int get_erased_pattern_value(Heuristics* heuristics, int8_t pattern[7], int start, uint8_t current_id) {
    int8_t last_move_value = pattern[3];
    pattern[3] = 0;
    int erased_value = get_pattern_value(heuristics, pattern, start, current_id);
    pattern[3] = last_move_value;
    return erased_value;
}


int heuristic_analysis(Game* game, Heuristics* heuristics, Coord last_move, uint8_t current_id, Coord* forced_move) {
    Board* board = get_board(game);
    int result = get_position_heuristic(heuristics, last_move);
    for (int direction = 0; direction < 3; direction++) {
        int8_t pattern[7] = {-1, -1, -1, -1, -1, -1, -1};
        fill_pattern(board, last_move, direction, pattern);
        int current_pattern_index = find_pattern_start(pattern);
        while (current_pattern_index < 4) {
            int value = get_pattern_value(heuristics, pattern, current_pattern_index, current_id);
            if (value == WIN || value == LOSS) {
                return value;
            }
            if (value == FORCED_MOVE_VALUE) {
                if (coord_is_valid(*forced_move)) {
                    return 1000;
                }
                Coord forced = get_forced_move_from_pattern(board, pattern, last_move, current_pattern_index, direction);
                my_assert(get_id_of_move(game, forced) == 0, "heuristic_analysis: forced move has non zero id!");
                my_assert(forced.x != last_move.x || forced.y != last_move.y,
                          "heuristic_analysis: forced move is last move!");
                set_coord(forced_move, forced);
            }
            result += value;
            int erased = get_erased_pattern_value(heuristics, pattern, current_pattern_index, current_id);
            my_assert(erased != WIN && erased != LOSS, "heuristic_analysis erased is WIN/LOSS");
            result -= erased;
            current_pattern_index++;
        }
    }
    return result;
}


int get_position_heuristic(Heuristics* heuristics, Coord enemy_move) {
    return heuristics->position_heuristic[enemy_move.x][enemy_move.y];
}
