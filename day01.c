#include <stdio.h>
#include <stdlib.h>
#include "aoc_input.h"

static long
calc_recursive(long val) {
    val = val / 3 - 2;
    if (val >= 0)
        return val + calc_recursive(val);
    else
        return 0;
}

#ifndef TEST

int
main(int argc, char **argv) {
    long total_part1 = 0;
    long total_part2 = 0;

    AocInputReader *reader = aoc_input_reader_new("day01");
    if (reader == NULL)
        return EXIT_FAILURE;

    char *line;
    while ((line = aoc_input_reader_getline(reader)) != NULL) {
        long val = aoc_input_parse_num(line);
        if (val == PARSE_NUM_ERR)
            return EXIT_FAILURE;

        total_part1 += val / 3 - 2;
        total_part2 += calc_recursive(val);
    }

    printf("Part 1: total fuel = %ld\n", total_part1);
    printf("Part 2: total fuel = %ld\n", total_part2);

    g_object_unref(reader);
    return EXIT_SUCCESS;
}


#else

void
test_recursive_calc() {
    g_assert_cmpint(calc_recursive(14), ==, 2);
    g_assert_cmpint(calc_recursive(1969), ==, 966);
    g_assert_cmpint(calc_recursive(100756), ==, 50346);
}

int
main(int argc, char **argv) {
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/day01/test_recursive_calc", test_recursive_calc);

    return g_test_run();
}

#endif
