#include "aoc_input.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define COLS 25u
#define ROWS 6u
#define LAYER_SIZE (COLS * ROWS)
#define BLACK '0'
#define WHITE '1'
#define TRANSPARENT '2'

unsigned int
part1(const char *pixels) {
    unsigned int min_zeros = UINT_MAX;
    unsigned int result = 0;

    size_t pos = 0;
    unsigned int counts[3] = {0, 0, 0};

    for (const char *px = pixels; *px != '\0'; px++) {
        switch (*px) {
        case '0': counts[0]++; break;
        case '1': counts[1]++; break;
        case '2': counts[2]++; break;
        default: break;
        }

        if (++pos == LAYER_SIZE) {
            if (counts[0] < min_zeros) {
                min_zeros = counts[0];
                result = counts[1] * counts[2];
            }
            pos = 0;
            counts[0] = counts[1] = counts[2] = 0;
        }
    }

    return result;
}

char *
part2(const char *pixels) {
    char *image = calloc(LAYER_SIZE, sizeof(char));
    size_t i = 0;

    memset(image, TRANSPARENT, LAYER_SIZE);

    for (const char *px = pixels; *px != '\0'; px++) {
        if (*px != TRANSPARENT && image[i] == TRANSPARENT)
            image[i] = *px;

        if (++i == LAYER_SIZE)
            i = 0;
    }

    return image;
}

void
print_image(const char *image) {
    size_t i = 0;

    for (const char *px = image; *px != '\0'; px++) {
        printf("%c", *px == WHITE ? '#' : ' ');
        if (++i == COLS) {
            puts("");
            i = 0;
        }
    }
}

int
main(int argc, char **argv) {
    AocInputReader *reader = aoc_input_reader_new("day08");
    char *pixels = aoc_input_reader_getline(reader);

    unsigned int result = part1(pixels);
    printf("Part 1: result = %u\n", result);

    char *image = part2(pixels);
    printf("Part 2: message:\n");
    print_image(image);

    g_object_unref(reader);
    free(image);
    return 0;
}
