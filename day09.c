#include "aoc_input.h"
#include "aoc_error.h"
#include <glib.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
    GHashTable *mem;
    long ip;
    long rel_base;
    GQueue *input;
    GQueue *output;
    bool halted;
} Prog;

typedef enum {
    STATE_HALT,
    STATE_WAIT_INPUT,
    STATE_PROG_ERROR = -1
} ProgState;

typedef enum {
    ARG_MODE_POS = 0,
    ARG_MODE_IMM = 1,
    ARG_MODE_REL = 2
} ArgMode;

typedef struct {
    size_t pos;
    ArgMode mode;
} Arg;

typedef enum {
    OP_ADD = 1,
    OP_MUL = 2,
    OP_READ = 3,
    OP_WRITE = 4,
    OP_JUMP_TRUE = 5,
    OP_JUMP_FALSE = 6,
    OP_LESS = 7,
    OP_EQUAL = 8,
    OP_MV_BASE = 9,
    OP_HALT = 99
} Op;

static void prog_init(Prog *self, const GArray *prog);
static void prog_deinit(Prog *self);
static ProgState prog_run(Prog *self);
static long prog_memget(Prog *self, Arg arg);
static void prog_memset(Prog *self, Arg arg, long val);

#define queue_pop(queue) ((long)g_queue_pop_head(queue))
#define queue_push(queue, long_val) (g_queue_push_tail((queue), (gpointer)long_val))

static long
solve(const GArray *prog_data, long input) {
    Prog computer;
    prog_init(&computer, prog_data);

    queue_push(computer.input, input);
    ProgState rc = prog_run(&computer);
    if (rc != STATE_HALT)
        aoc_die("Unexpected program exit status (%d)\n", rc);
    long ret = queue_pop(computer.output);

    prog_deinit(&computer);
    return ret;
}

static ProgState
prog_run(Prog *self) {
    while (true) {
        Arg arg0 = {.pos = self->ip, .mode = ARG_MODE_IMM};
        long op_and_mode = prog_memget(self, arg0);
        Op op = op_and_mode % 100;

        ArgMode mode1 = (op_and_mode / 100) % 10;
        Arg arg1 = {.pos = self->ip + 1, .mode = mode1};

        ArgMode mode2 = (op_and_mode / 1000) % 10;
        Arg arg2 = {.pos = self->ip + 2, .mode = mode2};

        ArgMode mode3 = (op_and_mode / 10000) % 10;
        Arg arg3 = {.pos = self->ip + 3, .mode = mode3};

        long result;

        switch (op) {
        case OP_ADD:
            result = prog_memget(self, arg1) + prog_memget(self, arg2);
            prog_memset(self, arg3, result);
            self->ip += 4;
            break;

        case OP_MUL:
            result = prog_memget(self, arg1) * prog_memget(self, arg2);
            prog_memset(self, arg3, result);
            self->ip += 4;
            break;

        case OP_READ:
            if (g_queue_is_empty(self->input)) {
                return STATE_WAIT_INPUT;
            } else {
                result = queue_pop(self->input);
                prog_memset(self, arg1, result);
                self->ip += 2;
            }
            break;

        case OP_WRITE:
            result = prog_memget(self, arg1);
            queue_push(self->output, result);
            self->ip += 2;
            break;

        case OP_JUMP_TRUE:
            if (prog_memget(self, arg1) != 0)
                self->ip = prog_memget(self, arg2);
            else
                self->ip += 3;
            break;

        case OP_JUMP_FALSE:
            if (prog_memget(self, arg1) == 0)
                self->ip = prog_memget(self, arg2);
            else
                self->ip += 3;
            break;

        case OP_LESS:
            result = prog_memget(self, arg1) < prog_memget(self, arg2);
            prog_memset(self, arg3, result);
            self->ip += 4;
            break;

        case OP_EQUAL:
            result = prog_memget(self, arg1) == prog_memget(self, arg2);
            prog_memset(self, arg3, result);
            self->ip += 4;
            break;

        case OP_MV_BASE:
            self->rel_base += prog_memget(self, arg1);
            self->ip += 2;
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
prog_memget(Prog *self, Arg arg) {
    switch (arg.mode) {
    case ARG_MODE_POS:
    case ARG_MODE_REL:
        assert(arg.pos >= 0);
        arg.pos = (long)g_hash_table_lookup(self->mem, GSIZE_TO_POINTER((size_t)arg.pos));
        arg.pos += arg.mode == ARG_MODE_REL ? self->rel_base : 0;
        // fall through
    case ARG_MODE_IMM:
        assert(arg.pos >= 0);
        return (long)g_hash_table_lookup(self->mem, GSIZE_TO_POINTER((size_t)arg.pos));
    default:
        aoc_die("Invalid ArgMode %d\n", arg.mode);
    }
}

static void
prog_memset(Prog *self, Arg arg, long val) {
    if (arg.mode == ARG_MODE_IMM)
        aoc_die("ARG_MODE_IMM not allowed for set operation\n");

    assert(arg.pos >= 0);
    arg.pos = (long)g_hash_table_lookup(self->mem, GSIZE_TO_POINTER((size_t)arg.pos));
    arg.pos += arg.mode == ARG_MODE_REL ? self->rel_base : 0;
    assert(arg.pos >= 0);
    g_hash_table_insert(self->mem, GSIZE_TO_POINTER((size_t)arg.pos), (gpointer)val);
}

void
prog_init(Prog *self, const GArray *prog_data) {
    static_assert(sizeof(gpointer) >= sizeof(long), "gpointer size < long size");

    self->mem = g_hash_table_new(g_direct_hash, g_direct_equal);
    self->ip = 0;
    self->rel_base = 0;
    self->input = g_queue_new();
    self->output = g_queue_new();
    self->halted = false;

    for (gsize i = 0; i < prog_data->len; i++)
        g_hash_table_insert(self->mem, GSIZE_TO_POINTER(i), (gpointer)g_array_index(prog_data, long, i));
}

static void
prog_deinit(Prog *self) {
    g_hash_table_unref(self->mem);
    g_queue_free(self->input);
    g_queue_free(self->output);
}

static GArray *
parse_input() {
    AocInputReader *reader = aoc_input_reader_new("day09");
    if (reader == NULL)
        return NULL;

    GArray *values = g_array_new(FALSE, FALSE, sizeof(long));
    if (values == NULL)
        goto out;

    char *token;
    while ((token = aoc_input_reader_getdelim(reader, ',')) != NULL) {
        long val = aoc_input_parse_num(token);
        if (val == PARSE_NUM_ERR)
            aoc_die("Parse number error (%s)\n", token);

        g_array_append_val(values, val);
    }

out:
    g_object_unref(reader);
    return values;
}


int
main(int argc, char **argv) {
    GArray *prog_data = parse_input();
    if (prog_data == NULL)
        aoc_die("Error parsing the input\n");

    long result = solve(prog_data, 1);
    printf("Part 1: result = %ld\n", result);

    result = solve(prog_data, 2);
    printf("Part 2: result = %ld\n", result);

    g_array_free(prog_data, TRUE);
    return 0;
}
