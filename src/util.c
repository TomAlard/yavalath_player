#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "game.h"


void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Couldn't allocate %zu bytes of memory!\n", size);
        exit(1);
    }
    return ptr;
}


FILE* safe_fopen(char* filename, char* mode) {
    FILE* file = fopen(filename, mode);
    if (file == NULL) {
        fprintf(stderr, "Couldn't open file '%s'\n", filename);
        exit(1);
    }
    return file;
}


int min_(int a, int b) {
    return a < b? a : b;
}


int max_(int a, int b) {
    return a > b? a : b;
}


int clip(int a, int min_value, int max_value) {
    return max_(min_(a, max_value), min_value);
}


#define PLAYER1 1
#define PLAYER2 2
uint8_t opposite_id(uint8_t id) {
    return id == PLAYER1? PLAYER2 : PLAYER1;
}


uint8_t opposite_direction(uint8_t direction) {
    return direction + 3;
}


clock_t get_deadline(double time) {
    return clock() + time*CLOCKS_PER_SEC;
}


bool has_time_remaining(clock_t deadline) {
    return clock() < deadline;
}


bool value_is_win_loss(int value) {
    return value == WIN || value == LOSS;
}


uint64_t set_bit(uint64_t number, int index) {
    return number | (1ULL << index);
}


uint64_t clear_bit(uint64_t number, int index) {
    return number & (~(1ULL << index));
}


bool bit_at_index(uint64_t number, int index) {
    return (number >> index) & 1ULL;
}


uint64_t change_bit_to(uint64_t number, int bit_index, uint8_t bit_value) {
    return number ^ ((-bit_value ^ number) & (1ULL << bit_index));
}


void my_assert(bool condition, char* msg) {
    if (!condition) {
        fprintf(stderr, "Assertion failed: %s\n", msg);
        exit(1);
    }
}
