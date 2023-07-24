#include "aoc_input.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>

#define NOT_FOUND ((size_t)-1)

static size_t path_length(GArray *path, const char *planet);

static GHashTable *
parse_input() {
    AocInputReader *reader = aoc_input_reader_new("day06");
    if (reader == NULL)
        return NULL;

    GHashTable *orbits = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    if (orbits == NULL)
        goto out;

    char *line;
    GArray *tokens = NULL;
    while ((line = aoc_input_reader_getline(reader)) != NULL) {
        tokens = aoc_input_split_char(line, ")", tokens);
        assert(tokens->len == 2);

        g_hash_table_insert(orbits,
                            strdup(g_array_index(tokens, char *, 1)),
                            strdup(g_array_index(tokens, char *, 0)));
        g_array_remove_range(tokens, 0, tokens->len);
    }

    g_array_free(tokens, TRUE);
out:
    g_object_unref(reader);

    return orbits;
}

static unsigned int
part1(GHashTable *orbits) {
    unsigned int orbits_count = 0;

    gpointer key;
    GHashTableIter iter;
    g_hash_table_iter_init(&iter, orbits);

    while (g_hash_table_iter_next(&iter, &key, NULL)) {
        while (strcmp((char *)key, "COM")) {
            orbits_count++;
            key = g_hash_table_lookup(orbits, key);
        }
    }

    return orbits_count;
}


static unsigned int
part2(GHashTable *orbits) {
    const char *orig  = g_hash_table_lookup(orbits, "YOU");
    const char *dest = g_hash_table_lookup(orbits, "SAN");

    const char *ship = orig;
    GArray *path1 = g_array_new(FALSE, FALSE, sizeof(char *));
    while (strcmp(ship, "COM")) {
        ship = g_hash_table_lookup(orbits, ship);
        g_array_append_val(path1, ship);
    }

    ship = dest;
    size_t path1_len;
    size_t path2_len = 0;
    while ((path1_len = path_length(path1, ship)) == NOT_FOUND) {
        path2_len++;
        ship = g_hash_table_lookup(orbits, ship);
    }

    g_array_free(path1, TRUE);
    return path1_len + path2_len;
}

static size_t
path_length(GArray *path, const char *planet) {
    for (size_t i = 0; i < path->len; i++) {
        if (!strcmp(planet, g_array_index(path, char *, i)))
            return i + 1;
    }
    return NOT_FOUND;
}

int
main(int argc, char **argv) {
    GHashTable *orbits = parse_input();
    if (orbits == NULL)
        return EXIT_FAILURE;

    unsigned int orbits_count = part1(orbits);
    printf("Part 1: orbits count = %u\n", orbits_count);

    orbits_count = part2(orbits);
    printf("Part 2: orbits count = %u\n", orbits_count);

    g_hash_table_destroy(orbits);
    return EXIT_SUCCESS;
}
