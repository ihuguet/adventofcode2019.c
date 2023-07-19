#include <glib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "aoc_input.h"

typedef enum {
    OP_ADD = 1,
    OP_MUL = 2,
    OP_READ = 3,
    OP_WRITE = 4,
    OP_JUMP_TRUE = 5,
    OP_JUMP_FALSE = 6,
    OP_LESS = 7,
    OP_EQUAL = 8,
    OP_HALT = 99
} ProgOp;

typedef enum {
    ARG_MODE_POS = 0,
    ARG_MODE_IMM = 1
} ArgMode;

typedef struct {
    size_t pos;
    ArgMode mode;
} ProgArg;

static long prog_get(long *prog, ProgArg arg);
static void prog_set(long *prog, ProgArg arg, long val);

static GArray *
run_program(long *prog, long *input) {
    size_t pos = 0;
    GArray *output = g_array_new(FALSE, FALSE, sizeof(long));

    while (true) {
        long op_and_mode = prog[pos];
        ProgOp op = op_and_mode % 100;

        ArgMode mode1 = (op_and_mode / 100) % 10;
        ProgArg arg1 = {.pos = pos + 1, .mode = mode1};

        ArgMode mode2 = (op_and_mode / 1000) % 10;
        ProgArg arg2 = {.pos = pos + 2, .mode = mode2};

        ArgMode mode3 = (op_and_mode / 10000) % 10;
        ProgArg arg3 = {.pos = pos + 3, .mode = mode3};

        long result;

        switch (op) {
        case OP_ADD:
            result = prog_get(prog, arg1) + prog_get(prog, arg2);
            prog_set(prog, arg3, result);
            pos += 4;
            break;

        case OP_MUL:
            result = prog_get(prog, arg1) * prog_get(prog, arg2);
            prog_set(prog, arg3, result);
            pos += 4;
            break;

        case OP_READ:
            prog_set(prog, arg1, *input);
            input++;
            pos += 2;
            break;

        case OP_WRITE:
            result = prog_get(prog, arg1);
            g_array_append_val(output, result);
            pos += 2;
            break;

        case OP_JUMP_TRUE:
            if (prog_get(prog, arg1) != 0)
                pos = prog_get(prog, arg2);
            else
                pos += 3;
            break;

        case OP_JUMP_FALSE:
            if (prog_get(prog, arg1) == 0)
                pos = prog_get(prog, arg2);
            else
                pos += 3;
            break;

        case OP_LESS:
            result = prog_get(prog, arg1) < prog_get(prog, arg2);
            prog_set(prog, arg3, result);
            pos += 4;
            break;

        case OP_EQUAL:
            result = prog_get(prog, arg1) == prog_get(prog, arg2);
            prog_set(prog, arg3, result);
            pos += 4;
            break;

        case OP_HALT:
            return output;

        default:
            goto out;
        }
    }

out:
    g_array_free(output, TRUE);
    return NULL;
}

static long
prog_get(long *prog, ProgArg arg) {
    switch (arg.mode) {
    case ARG_MODE_POS:
        return prog[prog[arg.pos]];
    case ARG_MODE_IMM:
        return prog[arg.pos];
    default:
        fprintf(stderr, "Invalid ArgMode %d\n", arg.mode);
        exit(EXIT_FAILURE);
    }
}

static void
prog_set(long *prog, ProgArg arg, long val) {
    if (arg.mode != ARG_MODE_POS) {
        fprintf(stderr, "Invalid arg mode for prog_set (%d)\n", arg.mode);
        exit(EXIT_FAILURE);
    }
    prog[prog[arg.pos]] = val;
}

static GArray *
parse_input() {
    AocInputReader *reader = aoc_input_reader_new("day05");
    if (reader == NULL)
        return NULL;

    GArray *values = g_array_new(FALSE, FALSE, sizeof(long));
    if (values == NULL)
        goto out;

    char *token;
    while ((token = aoc_input_reader_getdelim(reader, ',')) != NULL) {
        long val = aoc_input_parse_num(token);
        if (val == PARSE_NUM_ERR) {
            fprintf(stderr, "Parse number error: %s\n", token);
            exit(EXIT_FAILURE);
        }

        g_array_append_val(values, val);
    }

out:
    g_object_unref(reader);
    return values;
}

int
main(int argc, char **argv) {
    GArray *prog_orig = parse_input();
    if (prog_orig == NULL) {
        fprintf(stderr, "Error parsing the input\n");
        return EXIT_FAILURE;
    }

    long *prog = (long *)g_array_free(g_array_copy(prog_orig), FALSE);
    long input_part1[] = {1};
    GArray *output = run_program(prog, input_part1);
    if (output == NULL) {
        fprintf(stderr, "Program error\n");
        return EXIT_FAILURE;
    }
    if (output->len == 0) {
        fprintf(stderr, "Output is empty\n");
        return EXIT_FAILURE;
    }

    printf("Part 1: diagnostic code = %ld\n", g_array_index(output, long, output->len - 1));
    g_array_free(output, TRUE);

    prog = (long *)g_array_free(prog_orig, FALSE);
    long input_part2[] = {5};
    output = run_program(prog, input_part2);
    if (output == NULL) {
        fprintf(stderr, "Program error\n");
        return EXIT_FAILURE;
    }
    if (output->len == 0) {
        fprintf(stderr, "Output is empty\n");
        return EXIT_FAILURE;
    }

    printf("Part 2: diagnostic code = %ld\n", g_array_index(output, long, output->len - 1));
    g_array_free(output, TRUE);
}
