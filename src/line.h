#ifndef YAVALATHCODINGAMEC_LINE_H
#define YAVALATHCODINGAMEC_LINE_H

#include "game.h"

void construct_line_through_coord(Coord coord, uint8_t direction, Coord line[7]);

uint8_t calculate_line_length(Game* game, Coord start, uint8_t direction);

#endif //YAVALATHCODINGAMEC_LINE_H
