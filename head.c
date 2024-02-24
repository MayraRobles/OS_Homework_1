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
int get_first_n_lines_from_stdin(char ***lines_ptr, size_t **lines_lengths_ptr,
				 size_t *lines_total_ptr, size_t num_lines);


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

	  if (get_first_n_lines_from_stdin(&lines, &lines_lengths,
					    &lines_total, num_lines) == 1) {
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
	if (get_first_n_lines_from_stdin(&lines, &lines_lengths,
					  &lines_total, num_lines) == 1) {
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
    print_lines(lines, lines_lengths, lines_total);
}

 

/* This function is head specific. It reads from standard input
   until the number of lines specified have been reached or
   until the end of file.
*/
int get_first_n_lines_from_stdin(char ***lines_ptr, size_t **lines_lengths_ptr,
				  size_t *lines_total_ptr, size_t num_lines) {
  char buffer[BUFFER_SIZE];
  ssize_t read_res;
  size_t amount_new_chars, i;
  char c;
  char *current_line;
  size_t current_line_size;
  size_t current_line_len;
  char *ptr;
  size_t tmp;
  size_t lines_size;
  char **ptrt;
  size_t *ptrtt;
  char **lines;
  size_t *lines_lengths;
  size_t lines_total;
  size_t current_total_lines;

  lines = NULL;
  lines_lengths = NULL;
  lines_size = (size_t) 0;
  lines_total = (size_t) 0;
  current_line = NULL;
  current_line_size = (size_t) 0;
  current_line_len = (size_t) 0;
  current_total_lines = (size_t) 0;


  for (;;) {
    /* Read the next chunk of data from standard input */
    read_res = read(0, buffer, sizeof(buffer));

    /* Handle errors */
    if (read_res < ((ssize_t) 0)) {
      fprintf(stderr, "Error while reading: %s\n", strerror(errno));
      /* Deallocate everything we allocated */
      __clean_up_memory(current_line, lines, lines_lengths, lines_total);
      return 1;
    }

    /* If we hit EOF, break out of the read loop */
    if (read_res == ((ssize_t) 0)) {
      write_less_number_of_lines_than_content_message("standard input");
      break;
    }

    /* Here, we have at least 1 new character */
    amount_new_chars = (size_t) read_res;

    /* Go over all new characters, checking if they start new lines or
       if we need to add them to the current line.
    */
    for (i=((size_t) 0); i < amount_new_chars; i++) {
      c = buffer[i];

      if (c == '\n') {
	current_total_lines++; // Increment line counter on newline
	//printf("CURRENT TOTAL LINES: %ld, NUM_LINES: %ld\n", current_total_lines, num_lines);
	if (current_total_lines >= num_lines) {
	  break;
	}
      }

      /* Put the character into the current line, even if it is a new
	 line character.
      */
      if ((current_line_len + ((size_t) 1)) > current_line_size) {
	/* We need to allocate memory or the reallocate memory */
	if (current_line == NULL) {
	  /* This is the first allocation for this line 
	     
	     Let's allocate a default length.

	  */
	  current_line_size = FIRST_ALLOCATION_SIZE;
	  ptr = (char *) malloc(current_line_size);
	  if (ptr == NULL) {
	    write_allocating_memory_error_message();
	    /* Deallocate everything we allocated */
	    __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	    return 1;
	  }
	  current_line = ptr;
	} else {
	  tmp = current_line_size + current_line_size;
	  if (tmp < current_line_size) {
	    /* Overflow on our multiplication by 2 */
	    fprintf(stderr, "Error: this system cannot handle lines this long.");
	    /* Deallocate everything we allocated */
	    __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	    return 1;
	  }
	  current_line_size = tmp;
	  ptr = (char *) realloc(current_line, current_line_size);
	  if (ptr == NULL) {
	    write_allocating_memory_error_message();
	    /* Deallocate everything we allocated */
	    __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	    return 1;
	  }
	  current_line = ptr;
	}
      }

      /* Here, we know that we have space for at least one 
	 character in current_line and that current_line has been 
	 allocated as needed.

	 We put the character c into the current line and 
	 increment the current line's length.
      */
      current_line[current_line_len] = c;
      current_line_len++;

      /* If the current character is a newline ('\n'), 
	 we need to end the line and add the line into 
	 an array of all lines.
      */
      if (c == '\n') {
	if ((lines_total + ((size_t) 1)) > lines_size) {
	  /* We need to allocate or reallocate memory */
	  if (lines == NULL) {
	    /* This is the first allocation */
	    lines_size = FIRST_ALLOCATION_LINE_ARRAY_SIZE;
	    lines = (char **) calloc(lines_size, sizeof(char *));
	    if (lines == NULL) {
	      write_allocating_memory_error_message();
	      /* Deallocate everything we allocated */
	      __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	      return 1;
	    }
	    lines_lengths = (size_t *) calloc(lines_size, sizeof(size_t));
	    if (lines_lengths == NULL) {
	      write_allocating_memory_error_message();
	      /* Deallocate everything we allocated */
	      __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	      return 1;
	    }
	  } else {
	    /* This is a reallocation */
	    tmp = lines_size + lines_size;
	    if (tmp < lines_size) {
	      /* Overflow on our multiplication by 2 */
	      fprintf(stderr, "Error: this system cannot handle that many lines.");
	      /* Deallocate everything we allocated */
	      __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	      return 1;
	    }
	    lines_size = tmp;
	    ptrt = (char **) reallocarray(lines, lines_size, sizeof(char *));
	    if (ptrt == NULL) {
	      write_allocating_memory_error_message();
	      /* Deallocate everything we allocated */
	      __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	      return 1;
	    }
	    lines = ptrt;
	    ptrtt = (size_t *) reallocarray(lines_lengths, lines_size, sizeof(size_t));
	    if (ptrtt == NULL) {
	      write_allocating_memory_error_message();
	      /* Deallocate everything we allocated */
	      __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	      return 1;
	    }
	    lines_lengths = ptrtt;
	  }
	}
	
	/* Here, we have an array of lines allocated, into 
	   which we can put our current line.
	*/
	lines[lines_total] = current_line;
	lines_lengths[lines_total] = current_line_len;
	lines_total++;
	current_line = NULL;
	current_line_len = (size_t) 0;
	current_line_size = (size_t) 0;
      }
    }
    
    if (current_total_lines >= num_lines) {
      break;
    }
    
  }

  /* Here, we have read in all lines from the input.

     Before spitting out all lines in reverse order on standard 
     output, we need to handle one special case.

     If the current line is started but not finished by a newline,
     we need to add that new line to the line and add the line to 
     the array of lines.
     
  */
  if (current_line != NULL) {
    /* The current line is started. Add a new line and the line to the
       lines array. 
    */
    if ((current_line_len + ((size_t) 1)) > current_line_size) {
      /* We need to allocate memory or the reallocate memory */
      if (current_line == NULL) {
	/* This is the first allocation for this line 
	     
	   Let's allocate a default length.

	*/
	current_line_size = FIRST_ALLOCATION_SIZE;
	ptr = (char *) malloc(current_line_size);
	if (ptr == NULL) {
	  write_allocating_memory_error_message();
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	  return 1;
	}
	current_line = ptr;
      } else {
	tmp = current_line_size + current_line_size;
	if (tmp < current_line_size) {
	  /* Overflow on our multiplication by 2 */
	  fprintf(stderr, "Error: this system cannot handle lines this long.");
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	  return 1;
	}
	current_line_size = tmp;
	ptr = (char *) realloc(current_line, current_line_size);
	if (ptr == NULL) {
	  write_allocating_memory_error_message();
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	  return 1;
	}
	current_line = ptr;
      }
    }

    /* Here, we know that we have space for at least one 
       character in current_line and that current_line has been 
       allocated as needed.

       We put a new line character into the current line and 
       increment the current line's length.
    */
    current_line[current_line_len] = '\n';
    current_line_len++;

    /* Put the last line into the array of lines */
    if ((lines_total + ((size_t) 1)) > lines_size) {
      /* We need to allocate or reallocate memory */
      if (lines == NULL) {
	/* This is the first allocation */
	lines_size = FIRST_ALLOCATION_LINE_ARRAY_SIZE;
	lines = (char **) calloc(lines_size, sizeof(char *));
	if (lines == NULL) {
	  write_allocating_memory_error_message();
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	  return 1;
	}
	lines_lengths = (size_t *) calloc(lines_size, sizeof(size_t));
	if (lines_lengths == NULL) {
	  write_allocating_memory_error_message();
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	  return 1;
	}
      } else {
	/* This is a reallocation */
	tmp = lines_size + lines_size;
	if (tmp < lines_size) {
	  /* Overflow on our multiplication by 2 */
	  fprintf(stderr, "Error: this system cannot handle that many lines.");
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	  return 1;
	}
	lines_size = tmp;
	ptrt = (char **) reallocarray(lines, lines_size, sizeof(char *));
	if (ptrt == NULL) {
	  write_allocating_memory_error_message();
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	  return 1;
	}
	lines = ptrt;
	ptrtt = (size_t *) reallocarray(lines_lengths, lines_size, sizeof(size_t));
	if (ptrtt == NULL) {
	  write_allocating_memory_error_message();
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	  return 1;
	}
	lines_lengths = ptrtt;
      }
    }
	
    /* Here, we have an array of lines allocated, into 
       which we can put our current line.
    */
    lines[lines_total] = current_line;
    lines_lengths[lines_total] = current_line_len;
    lines_total++;
    current_line = NULL;
    current_line_len = (size_t) 0;
    current_line_size = (size_t) 0;
  }

  /* Here, we have the array lines of lines. Each line has a length
     that is stored in lines_length.
  */
  if (current_line != NULL) free(current_line);

  *lines_ptr = lines;
  *lines_lengths_ptr = lines_lengths;
  *lines_total_ptr = lines_total;
  
  return 0;
}
