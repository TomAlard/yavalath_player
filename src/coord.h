#ifndef YAVALATHCODINGAMEC_COORD_H
#define YAVALATHCODINGAMEC_COORD_H

#include <stdbool.h>
#include <stdint.h>

#define INVALID_COORD {-1, -1}

typedef struct Coord {
    int8_t x;
    int8_t y;
} Coord;

bool coord_is_valid(Coord coord);

bool coord_is_valid_xy(int8_t x, int8_t y);

void set_coord(Coord* target, Coord source);

void set_coord_invalid(Coord* target);

bool coord_equals_xy(Coord c, int8_t x, int8_t y);

#endif //YAVALATHCODINGAMEC_COORD_H