#include "hexagon_grid.h"
#include "board.h"
#include "game.h"


// https://www.redblobgames.com/grids/hexagons
typedef struct CubeCoord {
    int8_t q;
    int8_t r;
    int8_t s;
} CubeCoord;


CubeCoord coord_to_cube(Coord coord) {
    const CubeCoord conversion_table[] = {
            {0, -4, 4},
            {1, -4, 3},
            {2, -4, 2},
            {3, -4, 1},
            {4, -4, 0},

            {-1, -3, 4},
            {0, -3, 3},
            {1, -3, 2},
            {2, -3, 1},
            {3, -3, 0},
            {4, -3, -1},

            {-2, -2, 4},
            {-1, -2, 3},
            {0, -2, 2},
            {1, -2, 1},
            {2, -2, 0},
            {3, -2, -1},
            {4, -2, -2},

            {-3, -1, 4},
            {-2, -1, 3},
            {-1, -1, 2},
            {0, -1, 1},
            {1, -1, 0},
            {2, -1, -1},
            {3, -1, -2},
            {4, -1, -3},

            {-4, 0, 4},
            {-3, 0, 3},
            {-2, 0, 2},
            {-1, 0, 1},
            {0, 0, 0},
            {1, 0, -1},
            {2, 0, -2},
            {3, 0, -3},
            {4, 0, -4},

            {-4, 1, 3},
            {-3, 1, 2},
            {-2, 1, 1},
            {-1, 1, 0},
            {0, 1, -1},
            {1, 1, -2},
            {2, 1, -3},
            {3, 1, -4},

            {-4, 2, 2},
            {-3, 2, 1},
            {-2, 2, 0},
            {-1, 2, -1},
            {0, 2, -2},
            {1, 2, -3},
            {2, 2, -4},

            {-4, 3, 1},
            {-3, 3, 0},
            {-2, 3, -1},
            {-1, 3, -2},
            {0, 3, -3},
            {1, 3, -4},

            {-4, 4, 0},
            {-3, 4, -1},
            {-2, 4, -2},
            {-1, 4, -3},
            {0, 4, -4}
    };
    return conversion_table[(BOARD_SQUARES - 1) - coord_to_index(coord)];
}


Coord cube_to_coord(CubeCoord cube) {
    Coord result = {cube.r + 4, cube.q + (4 + (cube.r <= 0? cube.r : 0))};
    return result;
}


Coord rotate_coord_clockwise(Coord coord) {
    CubeCoord cube = coord_to_cube(coord);
    CubeCoord rotated_cube = {-cube.r, -cube.s, -cube.q};
    return cube_to_coord(rotated_cube);
}


Coord rotate_coord_counter_clockwise(Coord coord) {
    CubeCoord cube = coord_to_cube(coord);
    CubeCoord rotated_cube = {-cube.s, -cube.q, -cube.r};
    return cube_to_coord(rotated_cube);
}


CubeCoord add_cubes(CubeCoord c1, CubeCoord c2) {
    CubeCoord result = {c1.q + c2.q, c1.r + c2.r, c1.s + c2.s};
    return result;
}


Coord coord_neighbor(Coord coord, uint8_t direction, bool reverse) {
    CubeCoord cube = coord_to_cube(coord);
    const CubeCoord cube_offsets[] = {{1, 0, -1}, {0, 1, -1}, {-1, 1, 0}, {-1, 0, 1}, {0, -1, 1}, {1, -1, 0}};
    CubeCoord cube_neighbor = add_cubes(cube, cube_offsets[direction + 3*reverse]);
    return cube_to_coord(cube_neighbor);
}
