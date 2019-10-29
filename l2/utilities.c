#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utilities.h"

// utilities.c -- general purpose C functions

// malloc with error handling
void* safe_malloc(int size)
{
   void* ret = malloc(size);
   if (ret == NULL)
   {
      fprintf(stderr, "(safe_malloc) ERROR: out of memory for malloc\n");
      exit(1); // exit on failure
   }
   return ret;
}

// realloc with error handling
void* safe_realloc(void* ptr, int size)
{
   void* new = realloc(ptr, size);
   if (new == NULL) // Out of memory
   {
      fprintf(stderr, "(safe_realloc) ERROR: out of memory for realloc\n");
      exit(1); // exit on failure
   }
   return new;
}
