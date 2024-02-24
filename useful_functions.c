#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "useful_functions.h"
#include "string_manipulation.h"
#include "error_messages.h"

#define CONTENT_DEFAULT_SIZE (((size_t) 16))

/* This function writes len bytes from the buffer buf
   to the file descriptor fd.

   It uses write() to accomplish this job.

   It returns 0 if everything went well.
   It returns a negative number (-1) otherwise.
*/
int my_write(int fd, const void *buf, size_t len) {
  size_t bytes_to_be_written;
  size_t bytes_already_written;
  size_t bytes_written_this_time;
  ssize_t write_res;

  /* Loop until all bytes have been written */
  for (bytes_already_written = (size_t) 0, bytes_to_be_written = len;
       bytes_to_be_written > (size_t) 0;
       bytes_already_written += bytes_written_this_time,
       bytes_to_be_written -= bytes_written_this_time) {
    
    /* Using write() function.

       Returns the number of bytes actually written if sucessful.
       Returns -1 if an error occurs.
    */
    write_res = write(fd, &(((char *)buf)[bytes_already_written]),
		      bytes_to_be_written);
    if (write_res < (ssize_t) 0) {
      /* Condition failure, write() returns a negative number*/
      return -1;
    }
    bytes_written_this_time = write_res;
  }
  return 0;
}


/* This function cleans up the memory */
void __clean_up_memory(char *current_line, char **lines,
			      size_t *lines_length, size_t lines_len) {
  size_t i;
  
  if (current_line != NULL) free(current_line);
  if (lines != NULL) {
    for (i=((size_t) 0); i<lines_len; i++) {
      free(lines[i]);
    }
    free(lines);
    free(lines_length);
  }
}



/* This function reads a file into memory */
int read_from_file(char *filename, char **content_ptr,
		   size_t *content_len_ptr, size_t num_lines) {
  int fd;
  char buffer[4096];
  char *content;
  size_t content_len;
  size_t content_size;
  ssize_t read_res;
  size_t bytes_read, i;
  char c;
  size_t t;
  void *ptr;

  size_t current_total_lines;
  
  /* Open the file for reading */
  fd = open(filename, O_RDONLY);
  if (fd < 0) {
    write_opening_error_message(filename);
    return 1;
  }
  
  /* Let's read the whole file into memory */
  content = NULL;
  content_len = (size_t) 0;
  content_size = (size_t) 0;
  current_total_lines = (size_t) 0;
  
  for (;;) {
    read_res = read(fd, buffer, sizeof(buffer));
    if (read_res < ((ssize_t) 0)) {
      write_opening_error_message(filename);
      if (content != NULL) free(content);
      if (close(fd) < 0) {
	fprintf(stderr, "Error at closing \"%s\": %s\n", filename, strerror(errno));
      }
      return 1;
    }
    
    if (read_res == ((ssize_t) 0))
      break;
    
    bytes_read = (size_t) read_res;
    for (i=((size_t) 0); i<bytes_read; i++) {
      c = buffer[i];
      if (c == '\n') {
	current_total_lines++; // Increment line counter on newline
	printf("CURRENT TOTAL LINES: %ld, NUM_LINES: %ld\n", current_total_lines, num_lines);
	if (current_total_lines >= num_lines) {
	  break;
	}
      }
      
      if ((content == NULL) ||
	  ((content_len + ((size_t) 1)) > content_size)) {
	if (content == NULL) {
	  t = CONTENT_DEFAULT_SIZE;
	  if ((content_len + ((size_t) 1)) > t) {
	    t = content_len + ((size_t) 1);
	  }
	  ptr = malloc(t);
	  if (ptr == NULL) {
	    write_allocating_memory_error_message();
	    if (content != NULL) free(content);
	    if (close(fd) < 0) {
	      fprintf(stderr, "Error at closing \"%s\": %s\n", filename, strerror(errno));
	    }
	    return 1;
	  }
	  content = (char *) ptr;
	  content_size = t;
	} else {
	  t = content_size + content_size;
	  if (t < content_size) {
	    fprintf(stderr, "Error at manipulating that much of data\n");
	    if (content != NULL) free(content);
	    if (close(fd) < 0) {
	      fprintf(stderr, "Error at closing \"%s\": %s\n", filename, strerror(errno));
	    }
	    return 1;
	  } 
	  ptr = realloc(content, t);
	  if (ptr == NULL) {
	    write_allocating_memory_error_message();
	    if (content != NULL) free(content);
	    if (close(fd) < 0) {
	      fprintf(stderr, "Error at closing \"%s\": %s\n", filename, strerror(errno));
	    }
	    return 1;
	  }
	  content = (char *) ptr;
	  content_size = t;
	}
      }
      content[content_len] = c;
      content_len++;
    }
    // Check the condition to break out of the outer loop here
    if (current_total_lines >= num_lines) {
        break;
    }
  }
  *content_ptr = content;
  *content_len_ptr = content_len;
  return 0;
}


int get_lines_from_file(char *filename, char ***lines_ptr, size_t **lines_lengths_ptr,
                        size_t *lines_total_ptr, size_t num_lines) {
    char **lines = NULL;
    size_t *lines_lengths = NULL;
    size_t lines_total = (size_t) 0;
    char *content = NULL;
    size_t content_len = 0;


    if (num_lines < 0) {
      printf("Negative\n");
      return 1;
    }
    
    // Read content from file
    if (read_from_file(filename, &content, &content_len, num_lines) != 0) {
        return 1; // Indicate failure
    }

    // Iterate through the content array
    size_t start = 0;
    for (size_t i = 0; i < content_len; ++i) {
        // Check for line break or end of content
        if (content[i] == '\n' || i == content_len - 1) {
            // Allocate memory for the line
            char **new_lines = realloc(lines, (lines_total + 1) * sizeof(char *));
            if (new_lines == NULL) {
                // Handle allocation failure
                free(content);
                free(lines); // Free previously allocated memory
                free(lines_lengths); // Free previously allocated memory
                return 1; // Indicate failure
            }
            lines = new_lines;

            size_t *new_lengths = realloc(lines_lengths, (lines_total + 1) * sizeof(size_t));
            if (new_lengths == NULL) {
                // Handle allocation failure
                free(content);
                free(lines); // Free previously allocated memory
                free(lines_lengths); // Free previously allocated memory
                return 1; // Indicate failure
            }
            lines_lengths = new_lengths;

            // Calculate length of the line
            size_t line_length = i - start + 1;

            // Allocate memory for the line content
            lines[lines_total] = malloc(line_length+1);
            if (lines[lines_total] == NULL) {
                // Handle allocation failure
                free(content);
                for (size_t j = 0; j < lines_total; ++j) {
                    free(lines[j]); // Free previously allocated memory
                }
                free(lines); // Free previously allocated memory
                free(lines_lengths); // Free previously allocated memory
                return 1; // Indicate failure
            }
            memcpy(lines[lines_total], &content[start], line_length - 1);
	    lines[lines_total][line_length] = '\n'; // Add newline character
            lines[lines_total][line_length - 1] = '\0'; // Null-terminate the string

            // Store the line length
            lines_lengths[lines_total] = line_length;

            // Update start index for next line
            start = i + 1;

            // Increment total lines counter
            lines_total++;
        }
    }

    // Free content as it's no longer needed
    free(content);

    // Now lines, lines_lengths, and lines_total contain the parsed lines
    *lines_ptr = lines;
    *lines_lengths_ptr = lines_lengths;
    *lines_total_ptr = lines_total;

    return 0;
}


/* This function reads the lines from standard input
   and then stores the lines in an array, as well as
   an array containing the lengths of each line.
*/
int get_lines_from_stdin(char ***lines_ptr, size_t **lines_lengths_ptr,
			 size_t *lines_total_ptr) {
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
    if (read_res == ((ssize_t) 0)) 
      break;

    /* Here, we have at least 1 new character */
    amount_new_chars = (size_t) read_res;

    /* Go over all new characters, checking if they start new lines or
       if we need to add them to the current line.
    */
    for (i=((size_t) 0); i<amount_new_chars; i++) {
      c = buffer[i];

      if (c == '\n') {
	current_total_lines++; // Increment line counter on newline
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



int print_lines_2(bool starts_from_beginning, char **lines,
	        size_t *lines_lengths, size_t lines_total) {
    size_t i, k, start, stop;

    if (starts_from_beginning) {
      start = 0;
    } else {
      //start = lines_total - num_lines_to_print;
      stop = lines_total;
    }

    /* We need to write the first num_lines lines to standard out.*/
    for (k = start; k < stop; k++) {
        if (my_write(1, lines[k], lines_lengths[k]) < 0) {
	fprintf(stderr, "Error while reading: %s\n", strerror(errno));

	/* Deallocate everything we allocated */
	for (i = 0; i < lines_total; i++) {
	  free(lines[i]);
	}
	free(lines);
	free(lines_lengths);
	return 1;
      }
    }

    // Free memory allocated for lines and lines_length
   for (i = 0; i < lines_total; i++) {
     free(lines[i]);
   }
   free(lines);
   free(lines_lengths);
   return 0;
}


int print_lines(char **lines, size_t *lines_lengths, size_t lines_total) {
    size_t i, k;

    for (k = 0; k < lines_total; k++) {
        if (my_write(1, lines[k], lines_lengths[k]) < 0) {
	  write_opening_error_message("");

	  /* Deallocate everything we allocated */
	  for (i = 0; i < lines_total; i++) {
	    free(lines[i]);
	  }
	  free(lines);
	  free(lines_lengths);
	  return 1;
	}
    }

    // Free memory allocated for lines and lines_length
   for (i = 0; i < lines_total; i++) {
     free(lines[i]);
   }
   free(lines);
   free(lines_lengths);
   return 0;
}

