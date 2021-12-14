#include "line.h"
#include "util.h"


Coord get_next_in_line(Coord position, uint8_t direction) {
    int use_negative_offset = direction >= 3;
    const Coord line_offsets_top[] = {{0, 1}, {1, 1}, {1, 0}};
    const Coord line_offsets_bottom[] = {{0, 1}, {1, 0}, {1, -1}};
    Coord offset = (position.x < (4 + use_negative_offset)? line_offsets_top : line_offsets_bottom)[direction % 3];
    int factor = -(use_negative_offset*2 - 1);
    Coord result = {position.x + offset.x*factor, position.y + offset.y*factor};

    if (!coord_is_valid(result)) {
        Coord invalid = INVALID_COORD;
        return invalid;
    }
    return result;
}


void get_line(Coord line[4], Coord start, uint8_t direction) {
    Coord current = start;
    for (int i = 0; i < 4; i++) {
        line[i] = current;
        current = get_next_in_line(current, direction);
    }
}


#define INVALID_LINE4 {INVALID_COORD, INVALID_COORD, INVALID_COORD, INVALID_COORD}
void construct_line_through_coord(Coord coord, uint8_t direction, Coord line[7]) {
    Coord backwards[] = INVALID_LINE4;
    Coord forwards[] = INVALID_LINE4;
    get_line(backwards, coord, opposite_direction(direction));
    get_line(forwards, coord, direction);

    // Copy the backward line in reversed order
    for (int i = 0; i < 4; i++) {
        line[i] = backwards[3-i];
    }

    // Copy the forward line in normal order, omitting the Coord coord to not have it twice
    for (int i = 5; i < 8; i++) {
        line[i-1] = forwards[i-4];
    }
}


uint8_t calculate_line_length(Board* board, Coord start, uint8_t direction) {
    uint8_t start_id = get_id(board, start);
    Coord current = {start.x, start.y};
    uint8_t result = 0;
    while (result < 4 && coord_is_valid(current) && get_id(board, current) == start_id) {
        result++;
        current = get_next_in_line(current, direction);
    }
    return result;
}