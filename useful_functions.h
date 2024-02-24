#ifndef MY_WRITE_H
#define MY_WRITE_H

#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFFER_SIZE                      (((size_t) 4096))
#define FIRST_ALLOCATION_SIZE            (((size_t) 16))
#define FIRST_ALLOCATION_LINE_ARRAY_SIZE (((size_t) 16))


int my_write(int fd, const void *buf, size_t len);

void __clean_up_memory(char *current_line, char **lines,
		       size_t *lines_length, size_t lines_len);

int read_from_file(char *filename, char **content_ptr,
		   size_t *content_len_ptr, size_t num_lines);

int get_lines_from_file(char *filename, char ***lines_ptr, size_t **lines_lengths_ptr,
                        size_t *lines_total_ptr, size_t num_lines);

int get_lines_from_stdin(char ***lines, size_t **lines_lengths, size_t *lines_total);

ssize_t convert_from_string_to_number(const char *str, char **endptr);

int print_lines_2(bool starts_from_beginning, char **lines,
		size_t *lines_lengths, size_t lines_total);

int my_strlen(char *str);

void write_conversion_error(char *invalid_number);

int print_lines(char **lines,
		size_t *lines_lengths, size_t lines_total);

void *get_chars_from_standard_input(size_t *total_chars);

size_t get_number_of_lines();

#endif

