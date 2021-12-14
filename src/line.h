#ifndef YAVALATHCODINGAMEC_LINE_H
#define YAVALATHCODINGAMEC_LINE_H

#include "board.h"

void get_line(Coord line[4], Coord start, uint8_t direction);

void construct_line_through_coord(Coord coord, uint8_t direction, Coord line[7]);

uint8_t calculate_line_length(Board* board, Coord start, uint8_t direction);

#endif //YAVALATHCODINGAMEC_LINE_H
