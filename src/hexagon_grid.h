#ifndef YAVALATHCODINGAMEC_HEXAGON_GRID_H
#define YAVALATHCODINGAMEC_HEXAGON_GRID_H

#include "coord.h"

Coord rotate_coord_clockwise(Coord coord);

Coord rotate_coord_counter_clockwise(Coord coord);

Coord coord_neighbor(Coord coord, uint8_t direction, bool reverse);

#endif //YAVALATHCODINGAMEC_HEXAGON_GRID_H
