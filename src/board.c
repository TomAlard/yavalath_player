#include "board.h"
#include "game.h"
#include "util.h"


typedef struct LinesIndices {
    uint8_t down_right_row;
    uint8_t down_right_col;
    uint8_t down_left_row;
    uint8_t down_left_col;
} LinesIndices;


typedef struct Board {
    int8_t horizontal_ids[BOARD_SIZE][BOARD_SIZE];
    int8_t down_right_ids[BOARD_SIZE][BOARD_SIZE];
    int8_t down_left_ids[BOARD_SIZE][BOARD_SIZE];
    Coord down_right_lines[BOARD_SIZE][BOARD_SIZE];
    Coord down_left_lines[BOARD_SIZE][BOARD_SIZE];
    LinesIndices indices[BOARD_SIZE][BOARD_SIZE];
} Board;


Coord get_next_in_line(Coord position, bool is_down_left) {
    const Coord line_offsets_top[] = {{1, 1}, {1, 0}};
    const Coord line_offsets_bottom[] = {{1, 0}, {1, -1}};
    Coord offset = (position.x < 4? line_offsets_top : line_offsets_bottom)[is_down_left];
    Coord result = {position.x + offset.x, position.y + offset.y};

    if (!coord_is_valid(result)) {
        Coord invalid = INVALID_COORD;
        return invalid;
    }
    return result;
}


void fill_line(Coord line[BOARD_SIZE], Coord start, bool is_down_left) {
    Coord current = start;
    for (int i = 0; i < BOARD_SIZE; i++) {
        line[i] = current;
        if (coord_is_valid(current)) {
            current = get_next_in_line(current, is_down_left);
        }
    }
}


void init_down_right_lines(Board* board) {
    for (int x = 4; x >= 0; x--) {
        Coord line_start = {x, 0};
        fill_line(board->down_right_lines[4-x], line_start, false);
    }

    for (int y = 1; y <= 4; y++) {
        Coord line_start = {0, y};
        fill_line(board->down_right_lines[4+y], line_start, false);
    }
}


void init_down_left_lines(Board* board) {
    for (int y = 0; y <= 4; y++) {
        Coord line_start = {0, y};
        fill_line(board->down_left_lines[y], line_start, true);
    }

    for (int x = 1; x <= 4; x++) {
        Coord line_start = {x, x+4};
        fill_line(board->down_left_lines[4+x], line_start, true);
    }
}


void init_indices(Board* board) {
    init_down_right_lines(board);
    init_down_left_lines(board);
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            Coord c = board->down_right_lines[row][col];
            LinesIndices* indices = &board->indices[c.x][c.y];
            indices->down_right_row = row;
            indices->down_right_col = col;

            c = board->down_left_lines[row][col];
            indices = &board->indices[c.x][c.y];
            indices->down_left_row = row;
            indices->down_left_col = col;
        }
    }
}


void init_ids(Board* board) {
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            board->down_right_ids[row][col] = -1;
            board->down_left_ids[row][col] = -1;
        }
    }

    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            Coord c = {x, y};
            int8_t id = coord_is_valid(c)? 0 : -1;
            board->horizontal_ids[x][y] = id;
            if (id != -1) {
                LinesIndices indices = board->indices[x][y];
                board->down_right_ids[indices.down_right_row][indices.down_right_col] = id;
                board->down_left_ids[indices.down_left_row][indices.down_left_col] = id;
            }
        }
    }
}


Board* init_board() {
    Board* board = safe_malloc(sizeof(Board));
    init_indices(board);
    init_ids(board);
    return board;
}


int8_t get_id(Board* board, Coord coord) {
    return board->horizontal_ids[coord.x][coord.y];
}


void set_id(Board* board, Coord coord, int8_t id) {
    board->horizontal_ids[coord.x][coord.y] = id;
    LinesIndices indices = board->indices[coord.x][coord.y];
    board->down_right_ids[indices.down_right_row][indices.down_right_col] = id;
    board->down_left_ids[indices.down_left_row][indices.down_left_col] = id;
}


int8_t* get_ids_line(Board* board, Coord coord, uint8_t direction, uint8_t* col) {
    int8_t* ids;
    if (direction == 0) {
        ids = board->horizontal_ids[coord.x];
        *col = coord.y;
    } else if (direction == 1) {
        LinesIndices indices = board->indices[coord.x][coord.y];
        ids = board->down_right_ids[indices.down_right_row];
        *col = indices.down_right_col;
    } else {
        LinesIndices indices = board->indices[coord.x][coord.y];
        ids = board->down_left_ids[indices.down_left_row];
        *col = indices.down_left_col;
    }
    return ids;
}


void fill_pattern(Board* board, Coord coord, uint8_t direction, int8_t pattern[7]) {
    pattern[3] = board->horizontal_ids[coord.x][coord.y];
    uint8_t col;
    int8_t* ids = get_ids_line(board, coord, direction, &col);
    for (int before = col-1; before >= max_(0, col-3); before--) {
        pattern[3 - col + before] = ids[before];
    }
    for (int after = col+1; after <= min_(BOARD_SIZE-1, col+3); after++) {
        pattern[3 - col + after] = ids[after];
    }
}


uint8_t calculate_line_length(Board* board, Coord start, uint8_t direction) {
    int8_t start_id = get_id(board, start);
    uint8_t col;
    int8_t* ids = get_ids_line(board, start, direction % 3, &col);
    int step = (direction < 3)*2 - 1;
    uint8_t result = 0;
    int index = col;
    while (result < 4 && index >= 0 && index < BOARD_SIZE && ids[index] == start_id) {
        index += step;
        result++;
    }
    return result;
}


Coord get_forced_move_from_pattern(Board* board, const int8_t pattern[7], Coord last_move, int pattern_index, uint8_t direction) {
    uint8_t col;
    get_ids_line(board, last_move, direction, &col);
    uint8_t forced_move_col = (pattern_index - 2 + col) + (pattern[pattern_index + 1] != 0);

    if (direction == 0) {
        Coord result = {last_move.x, forced_move_col};
        return result;
    }
    LinesIndices indices = board->indices[last_move.x][last_move.y];
    if (direction == 1) {
        return board->down_right_lines[indices.down_right_row][forced_move_col];
    }
    return board->down_left_lines[indices.down_left_row][forced_move_col];
}