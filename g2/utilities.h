#ifndef utilities_H_
#define utilities_H_

// utilities.h -- General purpose utilities, for basic operations

// malloc with error handling
void* safe_malloc(int size);

// realloc with error handling
void* safe_realloc(void* ptr, int size);

// Realloc *dest and strcpy source to *dest
char* safe_strcpy(char** dest, char* source);

#endif
