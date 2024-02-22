#include "string_manipulation.h"
#include <stdio.h>

/*
   This function computes the length of a string.
 
   Returns:
   - The length of the string.
   - -1 if the string is null.
 */
int my_strlen(char *str) {
  int length = 0;
  int i = 0;
  
  if (str != NULL) {
    while (str[i] != '\0') {
      length += 1;
      i += 1;
    }
    return length;
  }
  
  return -1;
}


/**/
int my_strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(const unsigned char *)str1 - *(const unsigned char *)str2;
}


ssize_t convert_from_string_to_number(const char *str, char **endptr) {
  printf("HELLO I AM HERE\n");
  ssize_t result = 0;
  int sign = 1;

  /* Skip leading whitespaces */
  while (*str == ' ') {
    str++;
  }

  /* Check for the sign */
  if (*str == '-') {
    sign = -1;
    str++;
  }

  /* Convert digits to numbers */
  while (*str >= '0' && *str <= '9') {
    /* Check for overflow */
    if (result > (SIZE_MAX - (*str - '0')) / 10) {
      /* Overflow detected */
      if (endptr != NULL) {
	*endptr = (char *)str;
      }
      return sign == 1 ? SIZE_MAX : 0; 
    }
    result = result * 10 + (*str - '0');
    str++;
  }
  /* Set endptr to point to the character after the last
     digit found
  */
  if (endptr != NULL) {
    *endptr = (char *)str;
  }

  printf("This is the result of converting from string %ld\n", result);
  return result * sign;
}

