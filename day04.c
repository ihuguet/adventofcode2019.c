#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define PASSWORD_MIN {2, 4, 8, 3, 4, 5}
#define PASSWORD_MAX {7, 4, 6, 3, 1, 5}

static bool is_valid_pwd_part1(uint8_t digits[6]);
static bool is_valid_pwd_part2(uint8_t digits[6]);
static bool next_pwd(uint8_t digits[6]);

int
main(int argc, char **argv) {
    unsigned int valid_pwds_part1 = 0;
    unsigned int valid_pwds_part2 = 0;
    uint8_t digits[6] = PASSWORD_MIN;

    do {
        if (is_valid_pwd_part1(digits))
            valid_pwds_part1++;
        if (is_valid_pwd_part2(digits))
            valid_pwds_part2++;
    } while (next_pwd(digits));

    printf("Part 1: number of valid passwords = %u\n", valid_pwds_part1);
    printf("Part 2: number of valid passwords = %u\n", valid_pwds_part2);
}

static bool
is_valid_pwd_part1(uint8_t digits[6]) {
    bool repeated = false;
    for (size_t i = 0; i < 5; i++) {
        if (digits[i] > digits[i + 1])
            return false;
        else if (digits[i] == digits[i + 1])
            repeated = true;
    }
    return repeated;
}

static bool
is_valid_pwd_part2(uint8_t digits[6]) {
    for (size_t i = 0; i < 5; i++) {
        if (digits[i] > digits[i + 1])
            return false;
    }

    for (size_t i = 0; i < 5;) {
        size_t j = i + 1;
        while (j < 6 && digits[i] == digits[j])
            j++;
        if (j - i == 2)
            return true;
        i = j;
    }

    return false;
}

static bool
next_pwd(uint8_t digits[6]) {
    static uint8_t max[6] = PASSWORD_MAX;

    // increase 1
    for (size_t i = 5; i >= 0; i--) {
        digits[i] += 1;
        if (digits[i] > 9)
            digits[i] = 0;
        else
            break;
    }

    // advance to next number with non decreasing digits
    for (size_t i = 0; i < 5; i++) {
        if (digits[i] > digits[i + 1])
            digits[i + 1] = digits[i];
    }

    // check if digits <= max
    for (size_t i = 0; i < 6; i++) {
        if (digits[i] > max[i])
            return false;
        else if (digits[i] < max[i])
            return true;
    }
    return true;
}
