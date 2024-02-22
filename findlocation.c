#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "useful_functions.h"

int main(int argc, char **argv) {
  char *filename = "nanpa"; // Assuming nanpa is the file name
  int fd;
  off_t res_lseek;
  size_t file_size;
  void *ptr;

  /* Open the file for reading */
  fd = open(filename, O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "Error at opening \"%s\": %s\n", filename, strerror(errno));
    return 1;
  }

  /* Figure out the length of the file, as it appears for read() calls.

     The size on disc my be less, due to on-the-fly compression.

     We use lseek() for this purpose.

  */
  res_lseek = lseek(fd, ((off_t) 0), SEEK_END);
  if (res_lseek < ((off_t) 0)) {
    fprintf(stderr, "Error at using lseek on \"%s\": %s\n", filename, strerror(errno));
    if (close(fd) < 0) {
      fprintf(stderr, "Error at closing \"%s\": %s\n", filename, strerror(errno));
    }
    return 1;
  }

  file_size = (size_t) res_lseek;

  /* Handle the special case of a file that is empty */
  if (file_size < ((size_t) 1)) {
    if (close(fd) < 0) {
      fprintf(stderr, "Error at closing \"%s\": %s\n", filename, strerror(errno));
      return 1;
    }
    return 0;
  }
  
  /* Map the content of the file described by fd into memory */
  ptr = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, ((off_t) 0));
  if (ptr == MAP_FAILED) {
    fprintf(stderr, "Error at mapping \"%s\" to memory: %s\n", filename, strerror(errno));
    if (close(fd) < 0) {
      fprintf(stderr, "Error at closing \"%s\": %s\n", filename, strerror(errno));
    }
    return 1;    
  }

  /* Now the contents of our file are mapped into memory.

     Here, we did 2 O(1) operations, namely lseek() and 
     mmap(). So we got the contents of the file into 
     memory in constant time.

     Write the contents of the file to standard output now.
  */
  if (my_write(1, ptr, file_size) < 0) {
    fprintf(stderr, "Error at writing: %s\n", strerror(errno));
    if (munmap(ptr, file_size) < 0) {
      fprintf(stderr, "Error at unmapping the file \"%s\": %s\n", filename, strerror(errno));
    }
    if (close(fd) < 0) {
      fprintf(stderr, "Error at closing \"%s\": %s\n", filename, strerror(errno));
    }
    return 1;
  }
  
  /* Unmap the memory */
  if (munmap(ptr, file_size) < 0) {
    fprintf(stderr, "Error at unmapping the file \"%s\": %s\n", filename, strerror(errno));
    if (close(fd) < 0) {
      fprintf(stderr, "Error at closing \"%s\": %s\n", filename, strerror(errno));
    }
    return 1;
  }
  
  /* Close the file descriptor */
  if (close(fd) < 0) {
    fprintf(stderr, "Error at closing \"%s\": %s\n", filename, strerror(errno));
    return 1;
  }

  /* Signal success */
  return 0;
}
