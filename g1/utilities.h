#ifndef utilities_H_
#define utilities_H_

// utilities.h -- General purpose utilities, for basic operations

// malloc with error handling
void* safe_malloc(int size);

// realloc with error handling
void* safe_realloc(void* ptr, int size);

// Print a string array
void print_str_array(char** array, int size);

// Print two string arrays next to each other
void print_2_str_array(char** array1, int size1, char** array2, int size2);

// Check for null pointers before running strcmp
int safe_strcmp(char* str1, char* str2);

// Realloc *dest and strcpy source to *dest
char* safe_strcpy(char** dest, char* source);

// Locate index of key in array
int get_array_index(char** array, int size, char* key);

#endif
