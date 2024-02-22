#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "useful_functions.h"

#define BUFFER_SIZE                      (((size_t) 4096))
#define FIRST_ALLOCATION_SIZE            (((size_t) 16))
#define FIRST_ALLOCATION_LINE_ARRAY_SIZE (((size_t) 16))

int main(int argc, char **argv) {
    int num_lines;
    int converted_num;
    char *endptr;

    char **lines;
    size_t *lines_lengths;
    size_t lines_total;

    lines = NULL;
    lines_lengths = NULL;
    lines_total = (size_t) 0;
    num_lines = 10;

    /* The next conditional statements make sure to
       handle all well-formed calls to head */

    switch (argc) {
      case 4:
	/* This means filename and number of lines are specified */
	if (strcmp(argv[1], "-n") == 0 ) {
	  /* argv[0]: The program name
	     argv[1]: The -n option
	     argv[2]: The number of lines argument
	     argv[3]: The file name
	  */
        } else if (strcmp(argv[2], "-n") == 0) {
	  /* argv[0]: The program name
	     argv[1]: The file name
	     argv[2]: The -n option
	     argv[3]: The file name
	  */
        } else {
	  /* Has four arguments, but not in the correct format*/
	  print_error_message_badly_formed_call("head");
	  return 1;
        }
        break;
      case 3:
	/* argv[0]: The program name
	   argv[1]: The -n option
	   argv[2]: The number of lines argument
        */
        converted_num = convert_from_string_to_number(argv[2], &endptr);

        if (endptr == argv[2]) {
	  /* No valid digits found in the string */
	  printf("No digits were found\n");
	  return 1;
	
        } else if (*endptr != '\0') {
	  /* A valid number mixed with invalid characters */
          printf("Invalid characters after number: %s\n", endptr);
	  return 1;
	
        } else if (converted_num < 0) {
	  /* If negative number of lines is entered do nothing*/
	  printf("Doing nothing because negative.\n");
	  return 0;
	
        } else if (converted_num >= UINT64_MAX) {
	  num_lines = converted_num;
	  printf("The number you entered is too large.\n");
	  return 1;
        } else {
	  /* Everything went well, number was converted
	     correctly and is within range
	  */
   	  num_lines = converted_num;
	  get_lines_from_standard_input(&lines, &lines_lengths, &lines_total);
        }
        break;
      case 2:
         if (strncmp(argv[1], "-n", 2) != 0) {
	    /* argv[0]: The program name
	       argv[1]: The file name
	    */
	   get_lines_from_file(argv[1], &lines, &lines_lengths, &lines_total);
	   
	 } else {
	   print_error_message_badly_formed_call("head");
	   return 1;
	 }
	 break;
      case 1:
        /* argv[0]: The program name
	   Default number of lines is 10
        */
	get_lines_from_standard_input(&lines, &lines_lengths, &lines_total);
	break;
	
      default:
	print_error_message_badly_formed_call("head");
        return 1;
        break;
    }
    
    print_certain_number_of_lines(num_lines, true, lines, lines_lengths, lines_total);
}

 
