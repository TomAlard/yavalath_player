#include <stdlib.h>
#include "coord.h"
#include "board.h"


bool coord_is_valid(Coord coord) {
    return coord_is_valid_xy(coord.x, coord.y);
}


bool coord_is_valid_xy(int8_t x, int8_t y) {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE - abs(x-4);
}


void set_coord(Coord* target, Coord source) {
    target->x = source.x;
    target->y = source.y;
}


void set_coord_invalid(Coord* target) {
    target->x = -1;
    target->y = -1;
}


bool coord_equals_xy(Coord c, int8_t x, int8_t y) {
    return c.x == x && c.y == y;
}