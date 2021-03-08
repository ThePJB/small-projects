#pragma once

#include <stdbool.h>

bool strings_equal(char *a, char *b);
bool strings_is_dec_int(char *a);
int strings_atoi(char *a);
void strings_itoa(char *a, int i);