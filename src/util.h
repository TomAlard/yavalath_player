#ifndef YAVALATHCODINGAMEC_UTIL_H
#define YAVALATHCODINGAMEC_UTIL_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define REPEAT(x) for (int _ = 0; _ < x; _++)

void* safe_malloc(size_t size);

FILE* safe_fopen(char* filename, char* mode);

int min_(int a, int b);

int max_(int a, int b);

int sign(int a);

uint8_t opposite_id(uint8_t id);

uint8_t opposite_direction(uint8_t direction);

clock_t get_deadline(double time);

bool has_time_remaining(clock_t deadline);

bool value_is_win_loss(int value);

uint64_t set_bit(uint64_t number, int index);

uint64_t clear_bit(uint64_t number, int index);

bool bit_at_index(uint64_t number, int index);

uint64_t change_bit_to(uint64_t number, int bit_index, uint8_t bit_value);

void my_assert(bool condition, char* msg);

#endif //YAVALATHCODINGAMEC_UTIL_H
