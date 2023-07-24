#include <glib.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "aoc_input.h"

typedef struct _Coord {
    long x;
    long y;
} Coord;

typedef struct _Move {
    Coord dir;
    unsigned long dist;
} Move;

static GHashTable * get_visited_points(GArray *moves);
static GArray * parse_moves(char *line);
static guint coord_hash(gconstpointer c);
static gboolean coord_equal(gconstpointer a, gconstpointer b);

int
main(int argc, char **argv) {
    AocInputReader *reader = aoc_input_reader_new("day03");

    char *line1 = aoc_input_reader_getline(reader);
    if (line1 == NULL)
        return EXIT_FAILURE;

    GArray *moves1 = parse_moves(line1);
    if (moves1 == NULL)
        return EXIT_FAILURE;

    char *line2 = aoc_input_reader_getline(reader);
    if (line2 == NULL)
        return EXIT_FAILURE;

    GArray *moves2 = parse_moves(line2);
    if (moves2 == NULL)
        return EXIT_FAILURE;

    GHashTable *points1 = get_visited_points(moves1);
    GHashTable *points2 = get_visited_points(moves2);

    long min_radial_dist = LONG_MAX;
    long min_steps = LONG_MAX;

    GHashTableIter iter;
    gpointer key;
    g_hash_table_iter_init(&iter, points1);
    while (g_hash_table_iter_next(&iter, &key, NULL)) {
        if (g_hash_table_contains(points2, key)) {
            Coord *coord = (Coord *)key;

            long radial_dist = labs(coord->x) + labs(coord->y);
            if (radial_dist < min_radial_dist)
                min_radial_dist = radial_dist;

            long steps = (unsigned long)g_hash_table_lookup(points1, key) +
                         (unsigned long)g_hash_table_lookup(points2, key);
            if (steps < min_steps)
                min_steps = steps;
        }
    }

    printf("Part 1: min radial dist = %ld\n", min_radial_dist);
    printf("Part 2: min steps = %ld\n", min_steps);

    g_hash_table_unref(points1);
    g_hash_table_unref(points2);
    g_array_unref(moves1);
    g_array_unref(moves2);
    g_object_unref(reader);
    return EXIT_SUCCESS;
}

static GHashTable *
get_visited_points(GArray *moves) {
    Coord pos = {.x = 0, .y = 0};
    unsigned long steps = 0;
    GHashTable *points = g_hash_table_new(coord_hash, coord_equal);

    for (size_t i = 0; i < moves->len; i++) {
        Move *mov = &g_array_index(moves, Move, i);

        while (mov->dist > 0) {
            pos.x += mov->dir.x;
            pos.y += mov->dir.y;
            steps++;
            mov->dist--;

            Coord *key = malloc(sizeof(Coord));
            *key = pos;
            if (!g_hash_table_contains(points, key))
                g_hash_table_insert(points, key, (gpointer)steps);
        }
    }

    return points;
}

static GArray *
parse_moves(char *line) {
    GArray *tokens = aoc_input_split_char(line, ",", NULL);
    if (tokens == NULL)
        return NULL;

    GArray *moves = g_array_sized_new(FALSE, FALSE, sizeof(Move), tokens->len);
    if (moves == NULL)
        goto out;

    size_t i;
    for (i = 0; i < tokens->len; i++) {
        char *token = g_array_index(tokens, char *, i);
        Move mov;

        switch (token[0]) {
        case 'U': mov.dir.x = 0; mov.dir.y = 1; break;
        case 'R': mov.dir.x = 1; mov.dir.y = 0; break;
        case 'D': mov.dir.x = 0; mov.dir.y = -1; break;
        case 'L': mov.dir.x = -1; mov.dir.y = 0; break;
        default:
            fprintf(stderr, "Invalid direction: %s\n", token);
            exit(EXIT_FAILURE);
        }

        mov.dist = aoc_input_parse_num(token + 1);
        if (mov.dist == PARSE_NUM_ERR) {
            fprintf(stderr, "Can't parse distance: %s\n", token);
            exit(EXIT_FAILURE);
        }

        g_array_append_val(moves, mov);
    }

out:
    g_array_free(tokens, TRUE);
    return moves;
}

static guint
coord_hash(gconstpointer c) {
    const Coord *coord = c;
    return ((coord->x & 0xFF) << 16) | (coord->y & 0xFF);
}

static gboolean
coord_equal(gconstpointer a, gconstpointer b) {
    const Coord *ca = a, *cb = b;
    return ca->x == cb->x && ca->y == cb->y;
}
