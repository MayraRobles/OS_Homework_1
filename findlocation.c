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

typedef struct {
  char prefix[7]; // 6 digits + null terminator
  char location[26]; // 25 characters + null terminator
  char newline;
} nanpa_entry_t;


// Function prototypes
static int compare_entries(char *prefix_a, char *prefix_b);
static int perform_binary_search(nanpa_entry_t *entries, size_t n, char *prefix);
static int perform_lookup_doit(nanpa_entry_t *entries, size_t n, char *prefix);
static int perform_lookup(void *ptr, size_t size, char *prefix);
void *map_file_to_memory(const char *filename, size_t *file_size);


int main(int argc, char **argv) {
    char *filename = "nanpa"; // Assuming nanpa is the file name
    char *prefix;
    size_t file_size;
    void *ptr;

    int converted_num;
    char *endptr;
    

    /* Handle all calls */
    switch(argc) {
      case 1:
	/* When called with only one argument, tool must fail
	   and display a usage message on standard error */
	badly_formed_findlocation_call_error_message();
	return 1;
      case 2:
	/* When called with two arguments, we read from standard
	   input to look for the location associated with the number
	   in the argument and display that location.*/
	converted_num = convert_from_string_to_number(argv[1], &endptr);
	if (argv[1] == endptr || *endptr != '\0') {
	  /* Conversion from string to number failed */
	  write_conversion_error_message(argv[1]);
	  return 1;
	}
	printf("%d\n", converted_num);
	break;
      case 3:
	/* When called with three arguments, we look up the number in a file
	     argv[0]: The program name
	     argv[1]: The file name
	     argv[2]: The number to look for
	*/
	converted_num = convert_from_string_to_number(argv[2], &endptr);
	if (argv[2] == endptr || *endptr != '\0') {
	  /* Conversion from string to number failed */
	  write_conversion_error_message(argv[2]);
	  return 1;
	}
	prefix = argv[2];
	
	/* Map the file into memory */
	ptr = map_file_to_memory(argv[1], &file_size);
	if (ptr == NULL) {
	  printf("Hell, something went wrong");
	  return 1; // Error occurred, exit with failure
	}
	/* Now the contents of our file are mapped into memory.
	   Here, we have the file contents in memory pointed to by 'ptr'
	   with 'file_size' bytes in size.
	   Perform a lookup of the prefix given in argument. */

	/**************************************************************************/
	nanpa_entry_t *entries = (nanpa_entry_t *)ptr; // Cast ptr to nanpa_entry_t pointer
	// Calculate the number of entries based on the file size and the size of each entry
	size_t num_entries = file_size / sizeof(nanpa_entry_t);
	// Iterate over each entry
	//for (size_t i = 0; i < num_entries; ++i) {
	  // Access individual fields of each entry
	// printf("Prefix: %s, Location: %s\n", entries[i].prefix, entries[i].location);
	//	}
	for (size_t i = 0; i < num_entries; ++i) {
	  // Remove newline character from the prefix
	  entries[i].prefix[strcspn(entries[i].prefix, "\n")] = '\0';

	  // Print the prefix and location for each entry
	  printf("Prefix: %s, Location: %s\n", entries[i].prefix, entries[i].location);
	}
	/************************************************************************/

       
	if (perform_lookup(ptr, file_size, prefix) < 0) {
	  fprintf(stderr, "Error at looking up \"%s\" in the file \"%s\"\n", prefix, filename);
	  if (munmap(ptr, file_size) < 0) {
	    fprintf(stderr, "Error at unmapping the file \"%s\": %s\n", filename, strerror(errno));
	  }
	  return 1;
	  } 

	/* Unmap the memory */
	if (munmap(ptr, file_size) < 0) {
	  fprintf(stderr, "Error at unmapping the file \"%s\": %s\n", filename, strerror(errno));
	  return 1;
	}
	break;
      default:
	/* When called with more arguments, the tool fails and
	   and displays a usage message on standard error */
	badly_formed_findlocation_call_error_message();
	return 1;
    }

    return 0;
}


static int compare_entries(char *prefix_a, char *prefix_b) {
  return strcmp(prefix_a, prefix_b);
}


static int perform_binary_search(nanpa_entry_t *entries, size_t n, char *prefix) {
  ssize_t l = 0, r = n - 1;
  
  while (l <= r) {
    ssize_t m = (l + r) / 2;
    int res = compare_entries(entries[m].prefix, prefix);
    
    if (res == 0) {
      printf("Location: %s\n", entries[m].location);
      return 0;
    } else if (res < 0) {
      l = m + 1;
    } else {
      r = m - 1;
    }
  }
  
  printf("Location not found for prefix %s\n", prefix);
  return 0;
}


static int perform_lookup_doit(nanpa_entry_t *entries, size_t n, char *prefix) {
  /* Really do the binary search */
  return perform_binary_search(entries, n, prefix);
}


static int perform_lookup(void *ptr, size_t size, char *prefix) {
  size_t n;

  /* Check that the entries size is a multiple of full entries */
  /* if ((size % sizeof(nanpa_entry_t)) != ((size_t) 0)) {
    fprintf(stderr, "The file is not properly formatted\n");
    return -1;
    }*/

  /* We know that it is a full list (it's the right multiple in
     size).

     Let's compute the number of entries in the list.
     
  */
  n = size / sizeof(nanpa_entry_t);

  /* Really do the lookup */
  return perform_lookup_doit((nanpa_entry_t *) ptr, n, prefix);
}



/* Function to map the content of a file into memory */
void *map_file_to_memory(const char *filename, size_t *file_size) {
    int fd;
    off_t res_lseek;
    void *ptr;

    /* Open the file for reading */
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Error at opening \"%s\": %s\n", filename, strerror(errno));
        return NULL;
    }

    /* Figure out the length of the file */
    res_lseek = lseek(fd, ((off_t) 0), SEEK_END);
    if (res_lseek < ((off_t) 0)) {
        fprintf(stderr, "Error at using lseek on \"%s\": %s\n", filename, strerror(errno));
        if (close(fd) < 0) {
            fprintf(stderr, "Error at closing \"%s\": %s\n", filename, strerror(errno));
        }
        return NULL;
    }

    *file_size = (size_t) res_lseek;

    /* Map the content of the file described by fd into memory */
    ptr = mmap(NULL, *file_size, PROT_READ, MAP_SHARED, fd, ((off_t) 0));
    if (ptr == MAP_FAILED) {
        fprintf(stderr, "Error at mapping \"%s\" to memory: %s\n", filename, strerror(errno));
        if (close(fd) < 0) {
            fprintf(stderr, "Error at closing \"%s\": %s\n", filename, strerror(errno));
        }
        return NULL;
    }

    /* Close the file descriptor */
    if (close(fd) < 0) {
        fprintf(stderr, "Error at closing \"%s\": %s\n", filename, strerror(errno));
        return NULL;
    }

    return ptr;
}

