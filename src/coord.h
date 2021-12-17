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

void set_coord(Coord* target, Coord source);

void set_coord_invalid(Coord* target);

bool coord_equals_xy(Coord c, int8_t x, int8_t y);

bool coord_equals(Coord c1, Coord c2);

#endif //YAVALATHCODINGAMEC_COORD_H
