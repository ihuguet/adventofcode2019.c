#include <stdio.h>
#include "aoc_input.h"

#define INPUT_FILE_PATH_SIZE sizeof("dayXX.txt")

struct _AocInputReader {
    GObject parent;
    FILE *file;
    GString *buffer;
};

G_DEFINE_TYPE(AocInputReader, aoc_input_reader, G_TYPE_OBJECT)

AocInputReader *
aoc_input_reader_new(const char *dayXX) {
    char path[INPUT_FILE_PATH_SIZE];
    int rc = snprintf(path, INPUT_FILE_PATH_SIZE, "%s.txt", dayXX);
    if (rc < 0 || rc >= INPUT_FILE_PATH_SIZE) {
        fprintf(stderr, "%s: error formatting input file path (%s.txt)\n", __func__, dayXX);
        return NULL;
    }

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "%s: can't open file '%s'\n", __func__, path);
        return NULL;
    }

    AocInputReader *self = g_object_new(AOC_TYPE_INPUT_READER, NULL);
    self->file = f;
    return self;
}

GString *
aoc_input_reader_getline(AocInputReader *self) {
    ssize_t rc = getline(&self->buffer->str, &self->buffer->allocated_len, self->file);
    if (rc == -1)
        return NULL;

    if (self->buffer->str[rc - 1] == '\n') {
        self->buffer->str[rc - 1] = '\0'; // remove EOL
        self->buffer->len = rc - 1;
    } else {
        self->buffer->len = rc;
    }

    return self->buffer;
}

static void
aoc_input_reader_finalize(GObject *gobj) {
    g_assert(AOC_IS_INPUT_READER(gobj));

    AocInputReader *self = AOC_INPUT_READER(gobj);
    fclose(self->file);
    g_object_unref(self->buffer);
    G_OBJECT_CLASS(aoc_input_reader_parent_class)->finalize(gobj);
}

static void
aoc_input_reader_class_init(AocInputReaderClass *class) {
    GObjectClass *gobj = G_OBJECT_CLASS(class);
    gobj->finalize = aoc_input_reader_finalize;
}

static void
aoc_input_reader_init(AocInputReader *self) {
    self->buffer = g_string_new("");
}

long
aoc_input_parse_num(const char *str) {
    char *endptr;
    long val = strtol(str, &endptr, 10);
    if (endptr == str || *endptr != '\0')
        return PARSE_NUM_ERR;

    return val;
}
