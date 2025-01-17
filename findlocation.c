#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "error_messages.h"
#include "string_manipulation.h"
#include "useful_functions.h"

typedef struct {
  char prefix[6];    // 6 digits
  char location[25]; // 25 characters
  char newline;      // '\n'
} nanpa_entry_t;


// Function prototypes
static int perform_lookup(void *ptr, size_t size, char *word);
static int perform_lookup_doit(nanpa_entry_t *dict, size_t n, char *word);

int main(int argc, char **argv) {
    char *filename = "nanpa"; // Assuming nanpa is the file name
    size_t file_size;
    void *ptr;
    char *endptr, *prefix;
    

    /* Handle all calls */
    switch(argc) {
      case 1:
	/* When called with only one argument, tool must fail
	   and display a usage message on standard error */
	badly_formed_findlocation_call_error_message();
	return 1;
      case 2:
	/* When called with two arguments, we read from standard
	   input and look for the location associated with the number
	   in the argument and display that location.
	      argv[0]: The program name
	      argv[1]: The number to look for
	*/
	void *chars_ptr;
	size_t total_chars;
	
	convert_from_string_to_number(argv[1], &endptr);
	if (argv[1] == endptr || *endptr != '\0') {
	  /* Conversion from string to number failed */
	  write_conversion_error_message(argv[1]);
	  return 1;
	}
	chars_ptr = get_chars_from_standard_input(&total_chars);
	if (chars_ptr == NULL) {
	  return 1;
	}

                                                                           
	/* Now the contents of standard in  are mapped into memory.
	   Here, we have the standard in contents in memory pointed to by
	   'chars_ptr' with 'total_chars' bytes in size. */
	perform_lookup(chars_ptr, total_chars, argv[1]);
	
	break;
      case 3:
	/* When called with three arguments, we look up the number in a file
	     argv[0]: The program name
	     argv[1]: The file name
	     argv[2]: The number to look for
	*/
	prefix = argv[2];
	filename = argv[1];
	convert_from_string_to_number(argv[2], &endptr);
	if (argv[2] == endptr || *endptr != '\0') {
	  /* Conversion from string to number failed */
	  /* Maybe the three arguments are in this order:
	       argv[0]: The program name
	       argv[1]: The number to look for
	       argv[2]: The file name
	  */
	  prefix = argv[1];
	  filename = argv[2];
	  convert_from_string_to_number(argv[1], &endptr);
	  if (argv[1] == endptr || *endptr != '\0') {
	    badly_formed_findlocation_call_error_message();
	    return 1;
	  }
	} 
	
	/* Map the file into memory */
	ptr = map_file_to_memory(filename, &file_size);
	if (ptr == NULL) {
	  return 1;
	}
	/* Now the contents of our file are mapped into memory.
	   Here, we have the file contents in memory pointed to by 'ptr'
	   with 'file_size' bytes in size. */
	perform_lookup(ptr, file_size, prefix);
	
	if (munmap(ptr, file_size) < 0) {
	    fprintf(stderr, "Error at unmapping the file \"%s\": %s\n",
		    filename, strerror(errno));
	}
    }

    return 0;
}


static int perform_lookup(void *ptr, size_t size, char *word) {
  size_t n;
  /*
  printf("SIZE OF FILE: %zu\n", size);
  printf("SIZE OF ENTRY: %zu\n", sizeof(nanpa_entry_t));
  n = size / sizeof(nanpa_entry_t);
  printf("SIZE / NANPA ENTRY: %zu\n", n);
  n = (size % sizeof(nanpa_entry_t));
  printf("REMAINDER: %zu\n", n);
  */

  /* Check that the dictionary size is a multiple of full entries */
  if ((size % sizeof(nanpa_entry_t)) != ((size_t) 0)) {
    fprintf(stderr, "The file is not properly formatted\n");
    return -1;
  }

  /* We know that it is a full dictionary (it's the right multiple in
     size).
     Let's compute the number of entries in the dictionary.
     
  */
  n = size / sizeof(nanpa_entry_t);  

  /* Really do the lookup */
  return perform_lookup_doit((nanpa_entry_t *) ptr, n, word); 
}


static int perform_lookup_doit(nanpa_entry_t *dict, size_t n, char *word) {
  ssize_t l, r, m, nn;
  int res;

  nn = (ssize_t) n;
  if (nn < ((ssize_t) 0)) {
    fprintf(stderr, "The file is too large\n");
    return -1;
  }

  for (l=((ssize_t) 0), r=(nn-((ssize_t) 1)); l<=r;) {
    m = (l + r) >> 1;

    res = strncmp(dict[m].prefix, word, 6);

    if (res == 0) {
      my_write(1, "The location of ",  my_strlen("The location of "));
      my_write(1, dict[m].prefix, 6);
      my_write(1, " is ", my_strlen(" is "));
      my_write(1, dict[m].location, 25);
      my_write(1, "\n", my_strlen("\n"));
      return 0;
    }
    if (res < 0) {
      l = m + ((ssize_t) 1);
    } else {
      r = m - ((ssize_t) 1);
    }
  }
  printf("The word %s does not figure in the dictionary\n", word);
  return 0;
}
