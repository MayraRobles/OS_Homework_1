#ifndef MY_FUNCTIONS_H
#define MY_FUNCTIONS_H

#include <stddef.h>
#include <limits.h>
#include <stdint.h>
#include <sys/types.h>


int my_strlen(char *str);
int my_strcmp(const char *str1, const char *str2);
ssize_t convert_from_string_to_number(const char *str, char **endptr);

#endif /* MY_FUNCTIONS_H */
