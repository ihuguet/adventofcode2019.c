#ifndef AOC_INPUT_H_
#define AOC_INPUT_H_

#include <glib-object.h>
#include <limits.h>

#define PARSE_NUM_ERR LONG_MIN

#define AOC_TYPE_INPUT_READER (aoc_input_reader_get_type())
G_DECLARE_FINAL_TYPE(AocInputReader, aoc_input_reader, AOC, INPUT_READER, GObject)

/**
 * Create an AOC input file reader
 */
AocInputReader *
aoc_input_reader_new(const char *dayXX);

/**
 * Return the next line, or NULL on error.
 * The InputReader keeps the ownership. Increase the refcount of the string or
 * it will be destroyed when the InputReader is destroyed.
 * Manipulating the string is allowed, but the data is valid only until the next
 * call to getline, when the storage of the string is reused and the data
 * overwritten.
 */
char *
aoc_input_reader_getline(AocInputReader *reader);

/**
 * Like aoc_input_reader_getline, but selecting other delimiter other than '\n'.
 * The same limitations and ownership considerations apply.
 */
char *
aoc_input_reader_getdelim(AocInputReader *reader, char delim);

/**
 * Take ownership of the string buffer
 * Later, when calling getline/getdelim a new buffer will be created
 */
char *
aoc_input_reader_steal_buffer(AocInputReader *reader);

/**
 * Return the number parsed from the string, or PARSE_NUM_ERR
 */
long
aoc_input_parse_num(const char *str);

/**
 * Split the string str at any of the chars contained by the string delim.
 * Append the tokens to the out_arr array. If out_arr is NULL, it is created.
 * The same array is returned. It only contains pointers to the original string,
 * so it doesn't own the data.
 */
GArray *
aoc_input_split_char(char *str, const char *delim, GArray *out_arr);

#endif
