#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "game.h"
#include "util.h"
#include "hexagon_grid.h"


typedef uint64_t BitBoard;


typedef enum Direction {
    HORIZONTAL,
    DOWN_RIGHT,
    DOWN_LEFT
} Direction;


typedef struct Board {
    BitBoard player1;
    BitBoard player2;
    uint64_t winning_masks[3][BOARD_SQUARES];
    uint64_t losing_masks[3][BOARD_SQUARES];
    uint64_t forcing_move_masks[2][3][BOARD_SQUARES];
    uint64_t great_move_masks[3][BOARD_SQUARES];
    uint64_t bad_move_masks[5][3][BOARD_SQUARES];
    uint8_t position_heuristic[BOARD_SQUARES];
} Board;


const uint8_t offsets[] = {0, 5, 11, 18, 26, 35, 43, 50, 56};
int coord_to_index(Coord coord) {
    return coord_is_valid(coord)
        ? (BOARD_SQUARES - 1) - (offsets[coord.x] + coord.y)
        : -1;
}


const Coord table[] = {
        {0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4},
        {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5},
        {2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {2, 5}, {2, 6},
        {3, 0}, {3, 1}, {3, 2}, {3, 3}, {3, 4}, {3, 5}, {3, 6}, {3, 7},
        {4, 0}, {4, 1}, {4, 2}, {4, 3}, {4, 4}, {4, 5}, {4, 6}, {4, 7}, {4, 8},
        {5, 0}, {5, 1}, {5, 2}, {5, 3}, {5, 4}, {5, 5}, {5, 6}, {5, 7},
        {6, 0}, {6, 1}, {6, 2}, {6, 3}, {6, 4}, {6, 5}, {6, 6},
        {7, 0}, {7, 1}, {7, 2}, {7, 3}, {7, 4}, {7, 5},
        {8, 0}, {8, 1}, {8, 2}, {8, 3}, {8, 4}
};
Coord index_to_coord(int index) {
    return table[(BOARD_SQUARES - 1) - index];
}


int next_index(int index, Direction direction) {
    return coord_to_index(coord_neighbor(index_to_coord(index), direction, false));
}


int prev_index(int index, Direction direction) {
    return coord_to_index(coord_neighbor(index_to_coord(index), direction, true));
}


uint64_t calculate_mask(int index, uint8_t value, Direction direction, int pattern_length) {
    bool bits[4];
    for (int i = 0; i < pattern_length; i++) {
        bits[i] = bit_at_index(value, pattern_length - 1 - i);
    }

    uint64_t mask = 0;
    int current_index = index;
    for (int i = pattern_length - 1; i >= 0; i--) {
        mask = change_bit_to(mask, current_index, bits[i]);
        current_index = prev_index(current_index, direction);
    }
    return mask;
}


Coord start_coord_of_row(Direction direction, int row) {
    Coord zero = {row, 0};
    if (direction == HORIZONTAL) {
        return zero;
    } if (direction == DOWN_RIGHT) {
        return rotate_coord_clockwise(zero);
    }
    Coord c = {zero.x <= 4? 0 : zero.x - 4, zero.x};
    return c;
}


void fill_patterns_direction(uint64_t masks[BOARD_SQUARES], uint8_t value, int pattern_length, Direction direction) {
    for (int row = 0; row < BOARD_SIZE; row++) {
        int index = coord_to_index(start_coord_of_row(direction, row));
        REPEAT(pattern_length - 1) {
            masks[index] = -1;  // sets all bits to 1
            index = next_index(index, direction);
        }
        int amount = (BOARD_SIZE - pattern_length + 1) - abs(row - 4);
        for (int i = 0; i < amount - 1; i++) {
            masks[index] = calculate_mask(index, value, direction, pattern_length);
            index = next_index(index, direction);
        }
        masks[index] = calculate_mask(index, value, direction, pattern_length);
    }
}


void fill_patterns(uint64_t masks[3][BOARD_SQUARES], uint8_t value, int pattern_length) {
    fill_patterns_direction(masks[HORIZONTAL], value, pattern_length, HORIZONTAL);
    fill_patterns_direction(masks[DOWN_RIGHT], value, pattern_length, DOWN_RIGHT);
    fill_patterns_direction(masks[DOWN_LEFT], value, pattern_length, DOWN_LEFT);
}


void set_position_heuristic(Board* board) {
    uint8_t position_heuristic[BOARD_SQUARES] = {
            0, 0, 0, 0, 0,
            0, 1, 1, 1, 1, 0,
            0, 1, 2, 2, 2, 1, 0,
            0, 1, 2, 3, 3, 2, 1, 0,
            0, 1, 2, 3, 4, 3, 2, 1, 0,
            0, 1, 2, 3, 3, 2, 1, 0,
            0, 1, 2, 2, 2, 1, 0,
            0, 1, 1, 1, 1, 0,
            0, 0, 0, 0, 0,
    };
    memcpy(board->position_heuristic, position_heuristic, sizeof(board->position_heuristic));
}


Board* init_board() {
    Board* board = safe_malloc(sizeof(Board));
    board->player1 = 0;
    board->player2 = 0;
    fill_patterns(board->winning_masks, 15, 4);  // 1111
    fill_patterns(board->forcing_move_masks[0], 13, 4);  // 1101
    fill_patterns(board->forcing_move_masks[1], 11, 4);  // 1011
    fill_patterns(board->great_move_masks, 9, 4);  // 1001
    fill_patterns(board->bad_move_masks[0], 5, 4);  // 0101
    fill_patterns(board->bad_move_masks[1], 10, 4);  // 1010
    fill_patterns(board->bad_move_masks[2], 6, 4);  // 0110
    fill_patterns(board->bad_move_masks[3], 12, 4);  // 1100
    fill_patterns(board->bad_move_masks[4], 3, 4);  // 0011
    fill_patterns(board->losing_masks, 7, 3);  // 111
    set_position_heuristic(board);
    return board;
}


uint8_t get_id(Board* board, Coord coord) {
    uint8_t index = coord_to_index(coord);
    int p1 = bit_at_index(board->player1, index);
    int p2 = bit_at_index(board->player2, index);
    return p1 | (p2 << 1);
}


void set_id(Board* board, Coord coord, uint8_t id) {
    uint8_t index = coord_to_index(coord);
    if (id == 0) {
        board->player1 = clear_bit(board->player1, index);
        board->player2 = clear_bit(board->player2, index);
    } else if (id == 1) {
        board->player1 = set_bit(board->player1, index);
        board->player2 = clear_bit(board->player2, index);
    } else {
        board->player1 = clear_bit(board->player1, index);
        board->player2 = set_bit(board->player2, index);
    }
}


bool mask_matches(BitBoard bit_board, uint64_t mask) {
    return (bit_board & mask) == mask;
}


bool mask_matches_heuristic(Board* board, BitBoard bit_board, BitBoard both_players,Direction direction, uint64_t mask,
                            int index) {
    uint64_t select_all_mask = board->winning_masks[direction][index];
    return ((bit_board & select_all_mask) == mask) && ((both_players & select_all_mask) == mask);
}


int calculate_true_value(Board* board, Coord last_move) {
    int index = coord_to_index(last_move);
    uint8_t id = get_id(board, last_move);
    BitBoard bit_board = id == 1? board->player1 : board->player2;

    bool line3_found = false;
    for (int direction = 0; direction < 3; direction++) {
        int current_index = index;
        for (int i = 0; i < 4 && current_index != -1; i++) {
            uint64_t winning_mask = board->winning_masks[direction][current_index];
            if (mask_matches(bit_board, winning_mask)) {
                return WIN;
            }
            uint64_t losing_mask = board->losing_masks[direction][current_index];
            if (mask_matches(bit_board, losing_mask)) {
                line3_found = true;
            }
            current_index = next_index(current_index, direction);
        }
    }
    return line3_found? LOSS : UNDECIDED;
}


int calculate_heuristic_value_at_index(Board* board, BitBoard bit_board, BitBoard both_players, Direction direction,
                                       int index, Coord* forced_move) {
    for (int j = 0; j < 2; j++) {
        uint64_t forcing_move_mask = board->forcing_move_masks[j][direction][index];
        if (mask_matches_heuristic(board, bit_board, both_players, direction, forcing_move_mask, index)) {
            if (coord_is_valid(*forced_move)) {
                return DOUBLE_FORCING;
            }
            if (j == 0) {
                *forced_move = index_to_coord(prev_index(index, direction));
            } else {
                *forced_move = index_to_coord(prev_index(prev_index(index, direction), direction));
            }
            return FORCING;
        }
    }

    uint64_t great_move_mask = board->great_move_masks[direction][index];
    if (mask_matches_heuristic(board, bit_board, both_players, direction, great_move_mask, index)) {
        return GREAT;
    }

    for (int j = 0; j < 5; j++) {
        uint64_t bad_move_mask = board->bad_move_masks[j][direction][index];
        if (mask_matches_heuristic(board, bit_board, both_players, direction, bad_move_mask, index)) {
            return BAD;
        }
    }
    return 0;
}


int calculate_heuristic_value(Board* board, Coord last_move, Coord* forced_move) {
    int index = coord_to_index(last_move);
    uint8_t id = get_id(board, last_move);
    BitBoard bit_board = id == 1? board->player1 : board->player2;
    BitBoard both_players = board->player1 | board->player2;

    int result = board->position_heuristic[index];
    for (int direction = 0; direction < 3; direction++) {
        int current_index = index;
        for (int i = 0; i < 4 && current_index != -1; i++) {
            result += calculate_heuristic_value_at_index(board, bit_board, both_players, direction, current_index,
                                                         forced_move);
            current_index = next_index(current_index, direction);
        }
    }
    return result;
}


uint8_t position_heuristic(Board* board, Coord move) {
    return board->position_heuristic[coord_to_index(move)];
}