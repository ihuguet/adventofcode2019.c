#ifndef AOC_ERROR_H_
#define AOC_ERROR_H_

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdnoreturn.h>

noreturn static inline void
aoc_die(const char *err_msg, ...) {
    va_list var_args;
    va_start(var_args, err_msg);
    vfprintf(stderr, err_msg, var_args);
    exit(EXIT_FAILURE);
};

#endif // AOC_ERROR_H_
