#include <unistd.h>
#include "string_manipulation.h"
#include "useful_functions.h"


/* This function prints a message to tell the user how
   to call head or tail correctly after they it has been
   called incorrectly.
*/
void write_badly_formed_call_error_message(char *head_or_tail) {
  my_write(STDERR_FILENO, "\nThis is not a well-formed call to ",
	   my_strlen("\nThis is not a well-formed call to "));
  my_write(STDERR_FILENO, head_or_tail, my_strlen(head_or_tail));
  my_write(STDERR_FILENO, ".\n", my_strlen(".\n"));
  my_write(STDERR_FILENO, "Here are some examples of valid calls:\n",
	   my_strlen("Here are some examples of valid calls:\n"));
  my_write(STDERR_FILENO, "./", my_strlen("./"));
  my_write(STDERR_FILENO, head_or_tail, my_strlen(head_or_tail));
  my_write(STDERR_FILENO, " -n 42 nanpa\n", my_strlen(" -n 42 nanpa\n"));
  my_write(STDERR_FILENO, "./", my_strlen("./"));
  my_write(STDERR_FILENO, head_or_tail, my_strlen(head_or_tail));
  my_write(STDERR_FILENO, " nanpa -n 42\n", my_strlen(" nanpa -n 42\n"));
  my_write(STDERR_FILENO, "./", my_strlen("./"));
  my_write(STDERR_FILENO, head_or_tail, my_strlen(head_or_tail));
  my_write(STDERR_FILENO, " -n 42\n", my_strlen(" -n 42\n"));
  my_write(STDERR_FILENO, "./", my_strlen("./"));
  my_write(STDERR_FILENO, head_or_tail, my_strlen(head_or_tail));
  my_write(STDERR_FILENO, " nanpa\n", my_strlen(" nanpa\n"));
  my_write(STDERR_FILENO, "./", my_strlen("./"));
  my_write(STDERR_FILENO, head_or_tail, my_strlen(head_or_tail));
  my_write(STDERR_FILENO, "\n", my_strlen("\n"));
}


/* This function prints a message to standard error indicating
   that the string provided was not able to be converted
   into a valid number.
 */
void write_conversion_error_message(char *invalid_number) {
  my_write(STDERR_FILENO, "head: invalid number: ‘",
	   my_strlen("head: invalid number: ‘"));
  my_write(STDERR_FILENO, invalid_number, my_strlen(invalid_number));
  my_write(STDERR_FILENO, "’\n", my_strlen("’\n"));
}


/* This function writes a message to standard error indicating
   that there was an error opening a file.
 */
void write_opening_error_message(char *filename) {
  my_write(STDERR_FILENO, "Error at opening ",
	   my_strlen("Error at opening "));
  my_write(STDERR_FILENO, filename, my_strlen(filename));
  my_write(STDERR_FILENO,  ".\n ", my_strlen(".\n"));
}


/* This function writes a message to standard error indicating
   that there was an error opening a file.
 */
void write_allocating_memory_error_message() {
  my_write(STDERR_FILENO, "Error at allocating memory\n",
	   my_strlen("Error at allocating memory\n"));
}


/* This function writes a message to the standard error when
   findlocation is called with only one argument. And displays
   a usage message.
*/
void badly_formed_findlocation_call_error_message() {
  my_write(STDERR_FILENO, "\nThis is not a well-formed call to findlocation",
           my_strlen("\nThis is not a well-formed call to findlocation.\n"));
  my_write(STDERR_FILENO, "\nHere are some examples of valid calls:\n",
           my_strlen("\nHere are some examples of valid calls:\n"));
  my_write(STDERR_FILENO, "./findlocation", my_strlen("./findlocation"));
  my_write(STDERR_FILENO, " <number_to_look_for>\n", my_strlen(" <number_to_look_for>\n"));
  my_write(STDERR_FILENO, "./findlocation", my_strlen("./findlocation"));
  my_write(STDERR_FILENO, " <filename> <number_to_look_for>\n",
	   my_strlen(" <filename> <number_to_look_for>\n"));
  my_write(STDERR_FILENO, "./findlocation", my_strlen("./findlocation"));
  my_write(STDERR_FILENO, " <number_to_look_for> <filename>\n\n",
	   my_strlen(" <number_to_look_for> <filename>\n\n"));
}

/* This function writes a message to standard error when the
   number of lines the user wants displayed are more than
   those provided in standard input.
*/
void write_less_number_of_lines_than_content_message(char *data) {
  my_write(STDERR_FILENO, "\nThe number of lines given as an argument ",
	   my_strlen("\n The number of lines given as an argument "));
  my_write(STDERR_FILENO, "exceeds the number of lines in ",
	   my_strlen("exceeds the number of lines in "));
  my_write(STDERR_FILENO, data, my_strlen(data));
  my_write(STDERR_FILENO, ".\n ", my_strlen(".\n"));
  my_write(STDERR_FILENO, "Here are the entire contents:\n",
	   my_strlen("Here are the entire contents:\n"));
}



/* This function writes a message to standard error indicating
   that there was an error when unmapping
 */
void write_unmapping_error_message(char *filename) {
  my_write(STDERR_FILENO,  "Error at unmapping the file ",
	   my_strlen("Error at unmapping the file "));
  my_write(STDERR_FILENO, filename, my_strlen(filename));
}


/* This function writes a message to standard error indicating
   that there was an error while reading.
 */
void write_opening_error_message_no_filename() {
  my_write(STDERR_FILENO,  "Error while reading file.\n ",
	   my_strlen("Error while reading file.\n"));
}




