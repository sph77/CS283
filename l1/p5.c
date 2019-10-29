#include <stdlib.h>
#include <stdio.h>
#include <time.h>

typedef struct ArrayList // Generic arraylist struct
{
   int* data; // Array of data to store
   // I flipped size and capacity around from the traditional use case, so just refer to the definitions provided below
   int size; // Number of elements the arraylist can store, max
   int capacity; // Number of elements currently in the array
} ArrayList;

// Method signatures
ArrayList* ArrayList_new(int n);
void ArrayList_delete(ArrayList* al);
void ArrayList_add(ArrayList* al, int element, int version);
void ArrayList_remove(ArrayList* al, int index);
int ArrayList_get(ArrayList* al, int index);

ArrayList* ArrayList_new(int n) // Initialize new arraylist with size n
{
   ArrayList* new = (ArrayList*) malloc(sizeof(ArrayList));
   new->data = (int*) malloc(n*sizeof(int));
   new->size = n;
   new->capacity = 0;
   return new;
}

void ArrayList_delete(ArrayList* al) // Delete arraylist, free the data and free the struct
{
   free(al->data);
   free(al);
}

void ArrayList_add(ArrayList* al, int element, int version) // Add an element to an arraylist
{
   while (al->capacity >= al->size) // Resize array if necessary
   {
      if (version == 1) // V1, increment size by 1
      {
         al->size++;
      }
      else if (version == 2) // V2, double size
      {
         al->size *= 2;
      }
      al->data = (int*) realloc(al->data, (al->size) * sizeof(int)); // realloc for data
   }
   // Capacity guaranteed < size here.
   *(al->data + al->capacity) = element; // Add the element to the arraylist
   al->capacity++; // Increment capacity
}

void ArrayList_remove(ArrayList* al, int index) // Remove element at index
{
   int i;

   if (index < 0 || index >= al->capacity) // Exit if invalid index
   {
      return;
   }

   al->capacity--; // Decrement capacity

   for (i=index; i < al->capacity; i++) // Shift every element past the index down 1
   {
      *(al->data + i) = *(al->data + i + 1);
   }

   *(al->data + al->capacity) = 0; // Set the last element to 0 (empty) since capacity is down 1
}

int ArrayList_get(ArrayList* al, int index) // Get element at index
{
   if (index < 0 || index >= al->capacity) // Exit with -1 if invalid index
   {
      return -1;
   }
   
   return *(al->data + index); // Get the value at index using pointer arithmetic
}

int main(int argc, char* argv[])
{
   int NUM_ELEMENTS = 100000; // Number of elements to test timing
   ArrayList* al, * test_add, * test_double;
   int n, v, i, remove_index;
   clock_t t;
   double t_add, t_double;

   if (argc < 2)
   {
      n = 10; // Set default size to 10 if no arg supplied
   }
   else
   {
      n = atoi(argv[1]); // Set size to first arg
      if (n <= 0) // n is invalid or not a number
      {
         return 1;
      }
   }
   al = ArrayList_new(n);

   for (i = 0; i < n; i++) // Generate n random numbers to add
   {
      v = rand() % 100 + 1;
      ArrayList_add(al, v, 1);
   }

   for (i = 0; i < n; i++) // Print the numbers using get
   {
      printf("ArrayList[%d] = %d\n", i, ArrayList_get(al, i));
   }
   
   remove_index = rand() % n; // Choose a random index to remove, print it
   ArrayList_remove(al, remove_index);
   printf("Removed index %d.\n", remove_index);

   for (i = 0; i < n-1; i++) // Print the new list with the missing index
   {
      printf("ArrayList[%d] = %d\n", i, ArrayList_get(al, i));
   }

   ArrayList_delete(al); // Free the arraylist

   // Timing

   test_add = ArrayList_new(n);
   test_double = ArrayList_new(n);

   // Time adding many elements, using increment (V1)
   t = clock();
   for (i = 0; i < NUM_ELEMENTS; i++)
   {
      ArrayList_add(test_add, i, 1); 
   }
   t = clock() - t;
   t_add = ((double)t)/CLOCKS_PER_SEC;
   printf("adding one more element per realloc took %f seconds for %d elements\n", t_add, NUM_ELEMENTS);

   // Time adding many elements, using doubling (V2)
   t = clock();
   for (i = 0; i < NUM_ELEMENTS; i++)
   {
      ArrayList_add(test_double, i, 2);
   }
   t = clock() - t;
   t_double = ((double)t)/CLOCKS_PER_SEC;
   printf("doubling the size per realloc took %f seconds for %d elements\n", t_double, NUM_ELEMENTS);

   // Generally doubling is faster, because you call realloc() less

   // Free the test lists
   ArrayList_delete(test_add);
   ArrayList_delete(test_double);

   return 0;
}
