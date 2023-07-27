#include "aoc_input.h"
#include <glib.h>
#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

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
    STATE_HALT,
    STATE_WAIT_INPUT,
    STATE_PROG_ERROR = -1
} ComputerState;

typedef struct {
    long *prog;
    long ip;
    GQueue *input;
    GQueue *output;
    bool halted;
} Computer;

typedef enum {
    ARG_MODE_POS = 0,
    ARG_MODE_IMM = 1
} ArgMode;

typedef struct {
    size_t pos;
    ArgMode mode;
} ProgArg;

typedef long solve_fn(const GArray *prog, long inputs[5]);

static void computer_init(Computer *self, const GArray *prog);
static void computer_deinit(Computer *self);
static ComputerState computer_run(Computer *self);
static long prog_get(long *prog, ProgArg arg);
static void prog_set(long *prog, ProgArg arg, long val);
static void swap(long *a, long *b);

#define queue_pop(queue) ((long)g_queue_pop_head(queue))
#define queue_push(queue, long_val) (g_queue_push_tail((queue), (gpointer)long_val))

static void
run_with_permutations(long inputs[5], size_t i,
                      solve_fn solve, const GArray *prog, long *max_result) {
    if (i == 5) {
        long result = solve(prog, inputs);
        if (result > *max_result)
            *max_result = result;

        return;
    }

    for (size_t j = i; j < 5; j++) {
        swap(inputs + i, inputs + j);
        run_with_permutations(inputs, i + 1, solve, prog, max_result);
        swap(inputs + i, inputs + j);
    }
}

static long
solve_part1(const GArray *prog, long inputs[5]) {
    long val = 0;
    Computer computer;

    for (size_t i = 0; i < 5; i++) {
        computer_init(&computer, prog);

        queue_push(computer.input, inputs[i]);
        queue_push(computer.input, val);
        ComputerState rc = computer_run(&computer);
        if (rc == STATE_PROG_ERROR)
            return LONG_MIN;
        val = queue_pop(computer.output);

        computer_deinit(&computer);
    }

    return val;
}

static long
solve_part2(const GArray *prog, long inputs[5]) {
    Computer computers[5];
    for (size_t i = 0; i < 5; i++) {
        computer_init(&computers[i], prog);
        queue_push(computers[i].input, inputs[i]);
    }

    long val;
    GQueue io_pipe = G_QUEUE_INIT;
    queue_push(&io_pipe, 0);

    while (true) {
        for (size_t i = 0; i < 5; i++) {
            if (computers[i].halted)
                return LONG_MIN;

            while (!g_queue_is_empty(&io_pipe))
                queue_push(computers[i].input, queue_pop(&io_pipe));

            ComputerState rc = computer_run(&computers[i]);
            if (rc == STATE_PROG_ERROR)
                return LONG_MIN;

            while (!g_queue_is_empty(computers[i].output))
                queue_push(&io_pipe, queue_pop(computers[i].output));

            if (i == 4 && rc == STATE_HALT) {
                val = io_pipe.length == 1 ? queue_pop(&io_pipe) : LONG_MIN;
                goto out;
            }
        }
    }
out:
    for (size_t i = 0; i < 5; i++)
        computer_deinit(&computers[i]);
    g_queue_clear(&io_pipe);
    return val;
}

static ComputerState
computer_run(Computer *self) {
    while (true) {
        long op_and_mode = self->prog[self->ip];
        ProgOp op = op_and_mode % 100;

        ArgMode mode1 = (op_and_mode / 100) % 10;
        ProgArg arg1 = {.pos = self->ip + 1, .mode = mode1};

        ArgMode mode2 = (op_and_mode / 1000) % 10;
        ProgArg arg2 = {.pos = self->ip + 2, .mode = mode2};

        ArgMode mode3 = (op_and_mode / 10000) % 10;
        ProgArg arg3 = {.pos = self->ip + 3, .mode = mode3};

        long result;

        switch (op) {
        case OP_ADD:
            result = prog_get(self->prog, arg1) + prog_get(self->prog, arg2);
            prog_set(self->prog, arg3, result);
            self->ip += 4;
            break;

        case OP_MUL:
            result = prog_get(self->prog, arg1) * prog_get(self->prog, arg2);
            prog_set(self->prog, arg3, result);
            self->ip += 4;
            break;

        case OP_READ:
            if (g_queue_is_empty(self->input)) {
                return STATE_WAIT_INPUT;
            } else {
                result = queue_pop(self->input);
                prog_set(self->prog, arg1, result);
                self->ip += 2;
            }
            break;

        case OP_WRITE:
            result = prog_get(self->prog, arg1);
            queue_push(self->output, result);
            self->ip += 2;
            break;

        case OP_JUMP_TRUE:
            if (prog_get(self->prog, arg1) != 0)
                self->ip = prog_get(self->prog, arg2);
            else
                self->ip += 3;
            break;

        case OP_JUMP_FALSE:
            if (prog_get(self->prog, arg1) == 0)
                self->ip = prog_get(self->prog, arg2);
            else
                self->ip += 3;
            break;

        case OP_LESS:
            result = prog_get(self->prog, arg1) < prog_get(self->prog, arg2);
            prog_set(self->prog, arg3, result);
            self->ip += 4;
            break;

        case OP_EQUAL:
            result = prog_get(self->prog, arg1) == prog_get(self->prog, arg2);
            prog_set(self->prog, arg3, result);
            self->ip += 4;
            break;

        case OP_HALT:
            self->halted = true;
            return STATE_HALT;

        default:
            return STATE_PROG_ERROR;
        }
    }
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

void
computer_init(Computer *self, const GArray *prog) {
    long *prog_clone = calloc(prog->len, sizeof(long));
    memcpy(prog_clone, prog->data, prog->len * sizeof(long));

    self->prog = prog_clone;
    self->ip = 0;
    self->input = g_queue_new();
    self->output = g_queue_new();
    self->halted = false;
}

static void
computer_deinit(Computer *self) {
    g_free(self->prog);
    g_queue_free(self->input);
    g_queue_free(self->output);
}

static void
swap(long *a, long *b) {
    long tmp = *a;
    *a = *b;
    *b = tmp;
}

static GArray *
parse_input() {
    AocInputReader *reader = aoc_input_reader_new("day07");
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
    GArray *prog = parse_input();
    if (prog == NULL) {
        fprintf(stderr, "Error parsing the input\n");
        return EXIT_FAILURE;
    }

    long max_result = 0;
    long inputs[] = {0, 1, 2, 3, 4};
    run_with_permutations(inputs, 0, solve_part1, prog, &max_result);
    printf("Part 1: max result = %ld\n", max_result);

    max_result = 0;
    long inputs2[] = {5, 6, 7, 8, 9};
    run_with_permutations(inputs2, 0, solve_part2, prog, &max_result);
    printf("Part 2: max result = %ld\n", max_result);

    g_array_free(prog, TRUE);
    return 0;
}
