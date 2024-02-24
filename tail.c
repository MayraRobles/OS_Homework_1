#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "useful_functions.h"
#include "string_manipulation.h"
#include "error_messages.h"


#define BUFFER_SIZE                      (((size_t) 4096))
#define FIRST_ALLOCATION_SIZE            (((size_t) 16))
#define FIRST_ALLOCATION_LINE_ARRAY_SIZE (((size_t) 16))

// Function prototype
int print_last_n_lines(char ***lines_ptr, size_t **lines_lengths_ptr,
		       size_t num_lines_total, size_t num_lines);

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
    num_lines = 10; // The default number of lines is 10
    

    /* The following switch statement makes sure to handle all
      correct calls to head and to display the appropriate error
      messages when it is a bad call.
    */
    switch (argc) {
      case 4:
	/* Four arguments means that the filename and number of
	   lines are specified */
	
	if (strcmp(argv[1], "-n") == 0) {
	  /* argv[0]: The program name
	     argv[1]: The -n option
	     argv[2]: The number of lines argument
	     argv[3]: The file name
	  */
	  
	  converted_num = convert_from_string_to_number(argv[2], &endptr);
	  if (argv[2] == endptr || *endptr != '\0') {
	     /* Conversion from string to number failed */
	    write_conversion_error_message(argv[2]);
	    return 1;
	  } else {
	    /* Conversion was successful */
	    num_lines = converted_num;
	  }

	  if (get_lines_from_file(argv[3], &lines, &lines_lengths,
				  &lines_total, num_lines) == 1) {
	    /* Failed to obtain the lines array from the file contents*/
	    return 1;
	  }
	  
        } else if (strcmp(argv[2], "-n") == 0) {
	  /* argv[0]: The program name
	     argv[1]: The file name
	     argv[2]: The -n option
	     argv[3]: The number of lines argument
	  */
	  
	  converted_num = convert_from_string_to_number(argv[3], &endptr);
	  if (argv[3] == endptr || *endptr != '\0') {
	    /* Conversion from string to number failed */
	    write_conversion_error_message(argv[3]);
	    return 1;
	  } else {
	    /* Conversion was successful */
	    num_lines = converted_num;
	  }

	  if (get_lines_from_file(argv[1], &lines, &lines_lengths,
				  &lines_total, num_lines) == 1) {
	    /* Failed to obtain the lines array from the file contents*/
	    return 1;
	  }
	  
        } else {
	  /* The four arguments are not provided in a format we expect*/
	  write_badly_formed_call_error_message("head");
	  return 1;
        }
        break;
      case 3:
	if (strcmp(argv[1], "-n") == 0) {
	  /* argv[0]: The program name
	     argv[1]: The -n option
	     argv[2]: The number of lines argument
	  */
	  
	  converted_num = convert_from_string_to_number(argv[2], &endptr);
	  
	  if (argv[2] == endptr || *endptr != '\0') {
	    /* Conversion from string to number failed */
	    write_conversion_error_message(argv[2]);
	    return 1;
	  } else {
	    /* Conversion was successful */
	    num_lines = converted_num;
	  }
	  if (get_lines_from_stdin(&lines, &lines_lengths,
				   &lines_total) == 1) {
	    /* Failed to obtain the lines array from standard input*/
	    return 1;
	  }
	  
	} else {
	   /* The three arguments are not provided in a format we expect*/
	   write_badly_formed_call_error_message("head");
	   return 1;
	}	
        break;
      case 2:
         if (strcmp(argv[1], "-n") != 0) {
	    /* argv[0]: The program name
	       argv[1]: The file name
	    */
	   
	   if (get_lines_from_file(
	          argv[1], &lines, &lines_lengths,
		  &lines_total, num_lines) == 1) {
	     /* Failed to obtain the lines array from the file contents*/
	     return 1;
	   }
	   
	 } else {
	   /* The two arguments are not provided in a format we expect*/
	   write_badly_formed_call_error_message("head");
	   return 1;
	 }
	 break;
      case 1:
        /* argv[0]: The program name
	   Default number of lines is 10
        */
	if (get_lines_from_stdin(&lines, &lines_lengths,
			         &lines_total) == 1) {
	  /* Failed to obtain the lines array from the standard input*/
	  return 1;
	}
	break;
	
      default:
	write_badly_formed_call_error_message("head");
        return 1;
        break;
    }

    /* If we reach this point it means that:
       
       1. We have a valid lines array that we obtained either
          from standard input or from a file.
       2. We have a valid number of lines provided as an argument
          or that remain unchanged with the default number.

       Now, we can simply print the specified number of lines.
    */
    printf("WE ARE ABOUT TO PRINT THE LINES\n");
    
    return print_last_n_lines(&lines, &lines_lengths, lines_total, num_lines);
}




/* This function is tail specific. It takes a pointer to a an array
   of strings and prints the last number of lines specified.
*/
int print_last_n_lines(char ***lines_ptr, size_t **lines_lengths_ptr,
		       size_t num_lines_total, size_t num_lines) {
  size_t i, k, start;
  if (num_lines > num_lines_total) {
    /* If the number fo lines  requested is more than the
       number of lines provided in stdin, print everything*/
    write_less_number_of_lines_than_content_message("standar input");
    return print_lines(*lines_ptr, *lines_lengths_ptr, num_lines_total);
  }
  
  start = num_lines_total - num_lines;
  /* We need to write the first num_lines lines to standard out.*/
  for (k = start; k < num_lines_total; k++) {
    if (my_write(1,(*lines_ptr)[k], (*lines_lengths_ptr)[k]) < 0) {
      fprintf(stderr, "Error while reading: %s\n", strerror(errno));

      /* Deallocate everything we allocated */
      for (i = 0; i < num_lines_total; i++) {
	free((*lines_ptr)[i]);
      }
     free(*lines_ptr);
     free(*lines_lengths_ptr);
      return 1;
    }
  }

  // Free memory allocated for lines and lines_length
  for (i = 0; i < num_lines_total; i++) {
    free((*lines_ptr)[i]);
  }
   free(*lines_ptr);
   free(*lines_lengths_ptr);
   return 0;
}

