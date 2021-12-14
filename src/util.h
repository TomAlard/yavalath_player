#ifndef YAVALATHCODINGAMEC_UTIL_H
#define YAVALATHCODINGAMEC_UTIL_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

void* safe_malloc(size_t size);

FILE* safe_fopen(char* filename, char* mode);

int min_(int a, int b);

int max_(int a, int b);

int clip(int a, int min_value, int max_value);

uint8_t opposite_id(uint8_t id);

uint8_t opposite_direction(uint8_t direction);

clock_t get_deadline(double time);

bool has_time_remaining(clock_t deadline);

void my_assert(bool condition, char* msg);

#endif //YAVALATHCODINGAMEC_UTIL_H
