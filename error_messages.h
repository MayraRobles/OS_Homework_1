#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <unistd.h>
#include <stddef.h>

void write_badly_formed_call_error_message(char *head_or_tail);

void write_conversion_error_message(char *invalid_number);

void write_opening_error_message(char *filename);

void write_allocating_memory_error_message();

void badly_formed_findlocation_call_error_message();

void write_less_number_of_lines_than_content_message(char *data);

void write_unmapping_error_message(char *filename);

void write_opening_error_message_no_filename();

#endif /* ERROR_HANDLING_H */








