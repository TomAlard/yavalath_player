#include <stdio.h>
#include "test_hexagon_grid.h"
#include "../src/coord.h"
#include "../src/hexagon_grid.h"
#include "../src/util.h"


typedef struct CoordPair {
    Coord given;
    Coord expected;
} CoordPair;


void test_rotate_coord_clockwise() {
    CoordPair test_cases[] = {
            {{0, 0}, {0, 4}},
            {{0, 2}, {2, 6}},
            {{2, 2}, {2, 4}},
            {{3, 3}, {3, 4}},
            {{4, 4}, {4, 4}},
            {{6, 2}, {4, 2}},
            {{8, 4}, {8, 0}},
            {{0, 1}, {1, 5}}
    };

    char* fail_msg = "test_rotate_coord_clockwise: Rotated coord doesn't match expected coord";
    int amount_of_test_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    for (int i = 0; i < amount_of_test_cases; i++) {
        CoordPair pair = test_cases[i];
        Coord rotated = rotate_coord_clockwise(pair.given);
        my_assert(coord_equals(rotated, pair.expected), fail_msg);
    }
}


void test_rotate_coord_counter_clockwise() {
    CoordPair test_cases[] = {
            {{0, 0}, {4, 0}},
            {{0, 2}, {2, 0}},
            {{2, 2}, {4, 2}},
            {{3, 3}, {4, 3}},
            {{4, 4}, {4, 4}},
            {{6, 2}, {6, 4}},
            {{8, 4}, {4, 8}},
            {{0, 1}, {3, 0}}
    };

    char* fail_msg = "test_rotate_coord_counter_clockwise: Rotated coord doesn't match expected coord";
    int amount_of_test_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    for (int i = 0; i < amount_of_test_cases; i++) {
        CoordPair pair = test_cases[i];
        Coord rotated = rotate_coord_counter_clockwise(pair.given);
        my_assert(coord_equals(rotated, pair.expected), fail_msg);
    }
}


void test_coord_neighbor() {
    CoordPair test_cases_direction0[] = {
            {{0, 0}, {0, 1}},
            {{4, 4}, {4, 5}},
            {{7, 2}, {7, 3}}
    };
    CoordPair test_cases_direction1[] = {
            {{0, 0}, {1, 1}},
            {{4, 4}, {5, 4}},
            {{7, 2}, {8, 2}}
    };
    CoordPair test_cases_direction2[] = {
            {{1, 1}, {0, 1}},
            {{4, 4}, {3, 4}},
            {{7, 2}, {6, 3}}
    };

    char* fail_msg = "test_coord_neighbor: Result is not expected coord";
    for (int direction = 0; direction < 3; direction++) {
        CoordPair* test_cases = direction == 0? test_cases_direction0
                                : direction == 1? test_cases_direction1
                                : test_cases_direction2;
        int amount_of_test_cases = 3;
        for (int i = 0; i < amount_of_test_cases; i++) {
            CoordPair pair = test_cases[i];
            Coord neighbor = coord_neighbor(pair.given, direction, false);
            my_assert(coord_equals(neighbor, pair.expected), fail_msg);
            Coord reverse_neighbor = coord_neighbor(pair.expected, direction, true);
            my_assert(coord_equals(reverse_neighbor, pair.given), fail_msg);
        }
    }
}


void run_rotation_tests() {
    fprintf(stderr, "test_rotate_coord_clockwise\n");
    test_rotate_coord_clockwise();
    fprintf(stderr, "test_rotate_coord_counter_clockwise\n");
    test_rotate_coord_counter_clockwise();
    fprintf(stderr, "test_coord_neighbor\n");
    test_coord_neighbor();
}