cmake_minimum_required(VERSION 3.17)
project(yavalathCodinGameC C)

set(CMAKE_C_STANDARD 99)

add_executable(yavalathCodinGameC src/util.c src/util.h src/coord.c src/coord.h src/game.c src/game.h src/transposition_table.c src/transposition_table.h src/heuristics.c src/heuristics.h src/minimax.c src/minimax.h src/main.c test/test_game.c test/test_game.h test/test_main.c test/test_main.h test/test_minimax.c test/test_minimax.h src/random.c src/random.h src/main.h src/board.c src/board.h src/opening_book.c src/opening_book.h src/rotation.c src/rotation.h test/test_rotation.c test/test_rotation.h)