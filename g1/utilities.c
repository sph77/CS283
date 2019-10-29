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

// Print string array
void print_str_array(char** array, int size)
{
   int i;
   for (i = 0; i < size; i++)
   {
      if (*(array+i) == NULL)
         printf("NULL");
      else
         printf("%s", *(array+i));

      if (i < size - 1)
         printf("|");
      else
         printf("\n");
   }
}

// Print two string arrays, joined together
void print_2_str_array(char** array1, int size1, char** array2, int size2)
{
   int i;
   for (i = 0; i < size1; i++) // First array
   {
      if (*(array1+i) == NULL)
         printf("NULL");
      else
         printf("%s", *(array1+i));
      
      printf("|");
   }
   for (i = 0; i < size2; i++) // Second array
   {
      if (*(array2+i) == NULL)
         printf("NULL");
      else
         printf("%s", *(array2+i));

      if (i < size2 - 1)
         printf("|");
      else
         printf("\n");
   }
}

// realloc and strcpy
// Assuming that *dest already points to something
char* safe_strcpy(char** dest, char* source)
{
   if (source == NULL) // Invalid source
      return NULL;

   *dest = safe_realloc(*dest, strlen(source)+1);
   return strcpy(*dest, source);
}

// check for NULL pointers before strcmp 
int safe_strcmp(char* str1, char* str2)
{
   if (str1 == NULL && str2 == NULL) // Both are NULL (equal)
      return 0;
   else if (str1 == NULL || str2 == NULL) // Only one is NULL (exclusive or)
      return -1;
   else
      return strcmp(str1, str2); // strcmp should work, both are not NULL
}

// get index of key in array
// return index on success, -1 on failure
int get_array_index(char** array, int size, char* key)
{
   int i;
   for (i = 0; i < size; i++)
   {
      if (safe_strcmp(*(array+i), key) == 0)
         return i;
   }
   return -1; // Key not in array
}
