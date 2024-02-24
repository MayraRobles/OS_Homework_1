#ifndef FILE_MAPPING_H
#define FILE_MAPPING_H

#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/* Function to map the content of a file into memory */
void *map_file_to_memory(const char *filename, size_t *file_size);

#endif /* FILE_MAPPING_H */
