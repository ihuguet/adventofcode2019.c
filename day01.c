#include <stdio.h>
#include <stdlib.h>
#include "aoc_input.h"

#ifndef TEST

int main() {
    GString *line;

    AocInputReader *reader = aoc_input_reader_new("day01");
    if (reader == NULL)
        return 1;

    while ((line = aoc_input_reader_getline(reader)) != NULL) {
        long val = aoc_input_parse_num(line->str);
        if (val == PARSE_NUM_ERR)
            return 2;

        printf("%ld\n", val);
    }

    return 0;
}

#else

void
test1() {
    g_assert(TRUE);
}

int
main(int argc, char **argv) {
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/day01/test1", test1);

    return g_test_run();
}

#endif
