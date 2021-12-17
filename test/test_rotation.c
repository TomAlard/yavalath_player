#include <stdio.h>
#include "test_rotation.h"
#include "../src/coord.h"
#include "../src/rotation.h"
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


void run_rotation_tests() {
    fprintf(stderr, "test_rotate_coord_clockwise\n");
    test_rotate_coord_clockwise();
    fprintf(stderr, "test_rotate_coord_counter_clockwise\n");
    test_rotate_coord_counter_clockwise();
}