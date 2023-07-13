#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "aoc_input.h"

enum ops {
    OP_ADD = 1,
    OP_MUL = 2,
    OP_HALT = 99
};

static void
solve(long *values) {
    size_t pos = 0;

    long op;
    while ((op = values[pos]) != OP_HALT) {
        long *input1 = &values[values[pos + 1]];
        long *input2 = &values[values[pos + 2]];
        long *dest = &values[values[pos + 3]];

        if (op == OP_ADD) {
            *dest = *input1 + *input2;
        } else if (op == OP_MUL) {
            *dest = *input1 * *input2;
        } else {
            fprintf(stderr, "Invalid operand '%ld' at pos '%lu'\n", op, (unsigned long)pos);
            exit(EXIT_FAILURE);
        }

        pos += 4;
    }
}

void
part1(GArray *values) {
    g_array_index(values, long, 1) = 12;
    g_array_index(values, long, 2) = 2;
    solve((long *)values->data);

    printf("Part 1: pos0 = %ld\n", g_array_index(values, long, 0));
    g_array_free(values, TRUE);
}

void
part2(GArray *original_values) {
    long *values = calloc(original_values->len, sizeof(long));

    long min = 0;
    long max = 32;

    while (TRUE) {
        for (long val1 = min; val1 < max; val1++) {
            for (long val2 = 0; val2 < max; val2++) {
                memcpy(values, original_values->data, original_values->len * sizeof(long));
                values[1] = val1;
                values[2] = val2;
                solve(values);

                if (values[0] == 19690720) {
                    printf("Part 2: result = %ld\n", 100 * val1 + val2);
                    goto out;
                }
            }
        }

        min = max;
        max = min * 2;
    }

out:
    g_array_free(original_values, TRUE);
    free(values);
}

static GArray *
parse_input() {
    AocInputReader *reader = aoc_input_reader_new("day02");
    if (reader == NULL)
        return NULL;

    GArray *values = g_array_new(FALSE, FALSE, sizeof(long));
    if (values == NULL)
        goto out;

    char *token;
    while ((token = aoc_input_reader_getdelim(reader, ',')) != NULL) {
        long val = aoc_input_parse_num(token);
        g_array_append_val(values, val);
    }

out:
    g_object_unref(reader);
    return values;
}


#ifndef TEST

int
main(int argc, char **argv) {
    GArray *values = parse_input();
    part1(g_array_copy(values));
    part2(values);

}

#else

void
assert_solve(long *input, long *expect, size_t size) {
    GArray *values = g_array_new(FALSE, FALSE, sizeof(long));
    g_array_append_vals(values, input, size);
    solve((long *)values->data);

    g_assert_cmpmem(values->data, values->len * sizeof(long), expect, size * sizeof(long));
}

void
test_add() {
    long input[] = {1,0,0,0,99};
    long expect[] = {2,0,0,0,99};
    assert_solve(input, expect, sizeof(input) / sizeof(long));
}

void
test_mul() {
    long input[] = {2,3,0,3,99};
    long expect[] = {2,3,0,6,99};
    assert_solve(input, expect, sizeof(input) / sizeof(long));
}

void
test_extra_positions() {
    long input[] = {2,4,4,5,99,0};
    long expect[] = {2,4,4,5,99,9801};
    assert_solve(input, expect, sizeof(input) / sizeof(long));
}

void
test_program() {
    long input[] = {1,1,1,4,99,5,6,0,99};
    long expect[] = {30,1,1,4,2,5,6,0,99};
    assert_solve(input, expect, sizeof(input) / sizeof(long));
}

int
main(int argc, char **argv) {
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/day02/test_add", test_add);
    g_test_add_func("/day02/test_mul", test_mul);
    g_test_add_func("/day02/test_extra_positions", test_mul);
    g_test_add_func("/day02/test_program", test_program);

    return g_test_run();
}

#endif
