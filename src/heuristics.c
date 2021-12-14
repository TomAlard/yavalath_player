#include <string.h>
#include "heuristics.h"
#include "util.h"
#include "line.h"


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


int find_line_start(Coord line[7]) {
    int line_start = 0;
    while (!coord_is_valid(line[line_start])) {
        line_start++;
    }
    return line_start;
}


void fill_pattern_ids(Board* board, int8_t pattern[7], Coord line[7], int line_start) {
    for (int i = line_start; i < 7 && coord_is_valid(line[i]); i++) {
        pattern[i] = get_id(board, line[i]);
    }
}


int get_erased_pattern_value(Heuristics* heuristics, int8_t pattern[7], int start, uint8_t current_id) {
    int8_t last_move_value = pattern[3];
    pattern[3] = 0;
    int erased_value = get_pattern_value(heuristics, pattern, start, current_id);
    pattern[3] = last_move_value;
    return erased_value;
}


#define INVALID_LINE7 {INVALID_COORD, INVALID_COORD, INVALID_COORD, INVALID_COORD, INVALID_COORD, INVALID_COORD, INVALID_COORD}
int heuristic_analysis(Board* board, Heuristics* heuristics, Coord last_move, uint8_t current_id, Coord* forced_move) {
    int result = get_position_heuristic(heuristics, last_move);
    for (int direction = 0; direction < 3; direction++) {
        Coord line[7] = INVALID_LINE7;
        construct_line_through_coord(last_move, direction, line);
        int current_line_index = find_line_start(line);
        int8_t pattern[7] = {-1, -1, -1, -1, -1, -1, -1};
        fill_pattern_ids(board, pattern, line, current_line_index);

        while (current_line_index < 4) {
            int value = get_pattern_value(heuristics, pattern, current_line_index, current_id);
            if (value == FORCED_MOVE_VALUE) {
                if (coord_is_valid(*forced_move)) {
                    return 1000;
                }
                Coord forced = line[current_line_index+1 + (pattern[current_line_index+1] != 0)];
                my_assert(forced.x != last_move.x || forced.y != last_move.y,
                          "heuristic_analysis: forced move is last move!");
                set_coord(forced_move, forced);
            }
            result += value;
            int erased = get_erased_pattern_value(heuristics, pattern, current_line_index, current_id);
            my_assert(erased != WIN && erased != LOSS, "heuristic_analysis erased is WIN/LOSS");
            result -= erased;
            current_line_index++;
        }
    }
    return result;
}


int get_position_heuristic(Heuristics* heuristics, Coord enemy_move) {
    return heuristics->position_heuristic[enemy_move.x][enemy_move.y];
}
