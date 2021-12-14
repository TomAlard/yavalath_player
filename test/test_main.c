#include <stdio.h>
#include "test_main.h"
#include "test_board.h"
#include "test_minimax.h"


void run_tests() {
    fprintf(stderr, "=====START TESTS=====\n");
    fprintf(stderr, "=====START BOARD TESTS=====\n");
    run_board_tests();
    fprintf(stderr, "=====START MINIMAX TESTS=====\n");
    run_minimax_tests();
    fprintf(stderr, "=====END TESTS=====\n\n");
}