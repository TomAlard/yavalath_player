#include <stdio.h>
#include "test_main.h"
#include "test_game.h"
#include "test_minimax.h"
#include "test_hexagon_grid.h"


void run_tests() {
    fprintf(stderr, "=====START TESTS=====\n");
    fprintf(stderr, "=====START ROTATION TESTS=====\n");
    run_rotation_tests();
    fprintf(stderr, "=====START GAME TESTS=====\n");
    run_game_tests();
    fprintf(stderr, "=====START MINIMAX TESTS=====\n");
    run_minimax_tests();
    fprintf(stderr, "=====END TESTS=====\n\n");
}