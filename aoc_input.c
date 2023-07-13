#include <stdio.h>
#include "aoc_input.h"

#define INPUT_FILE_PATH_SIZE sizeof("dayXX.txt")

struct _AocInputReader {
    GObject parent;
    FILE *file;
    char *buffer;
    size_t buff_used;
    size_t buff_capacity;
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

char *
aoc_input_reader_getline(AocInputReader *self) {
    return aoc_input_reader_getdelim(self, '\n');
}

char *
aoc_input_reader_getdelim(AocInputReader *self, char delim) {
    ssize_t rc = getdelim(&self->buffer, &self->buff_capacity, delim, self->file);
    if (rc == -1)
        return NULL;

    if (self->buffer[rc - 1] == delim) {
        self->buffer[rc - 1] = '\0'; // remove delimiter character
        self->buff_used = rc - 1;
    } else {
        self->buff_used = rc;
    }

    return self->buffer;
}

char *
aoc_input_reader_steal_buffer(AocInputReader *self) {
    char *buff = self->buffer;
    self->buffer = NULL;
    self->buff_used = 0;
    self->buff_capacity = 0;
    return buff;
}

static void
aoc_input_reader_finalize(GObject *gobj) {
    g_assert(AOC_IS_INPUT_READER(gobj));

    AocInputReader *self = AOC_INPUT_READER(gobj);
    fclose(self->file);
    free(self->buffer);
    G_OBJECT_CLASS(aoc_input_reader_parent_class)->finalize(&self->parent);
}

static void
aoc_input_reader_class_init(AocInputReaderClass *class) {
    GObjectClass *gobj = G_OBJECT_CLASS(class);
    gobj->finalize = aoc_input_reader_finalize;
}

static void
aoc_input_reader_init(AocInputReader *self) {
    self->buffer = NULL;
}

long
aoc_input_parse_num(const char *str) {
    char *endptr;
    long val = strtol(str, &endptr, 10);
    if (endptr == str || *endptr != '\0')
        return PARSE_NUM_ERR;

    return val;
}

GArray *
aoc_input_split_char(char *str, const char *delim) {
    GArray *tokens = g_array_new(FALSE, FALSE, sizeof(char *));
    if (tokens == NULL)
        return NULL;

    char *token = str;
    while (str != NULL) {
        token = strsep(&str, delim);
        g_array_append_val(tokens, token);
    }

    return tokens;
}
