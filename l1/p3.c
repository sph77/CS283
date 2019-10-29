#include <stdlib.h>
#include <stdio.h>

// https://en.wikipedia.org/wiki/Bubble_sort

void swap(int* a, int i, int j) // Generic swap function for int array a, locs i and j
{
   int temp = *(a + i);
   *(a + i) = *(a + j);
   *(a + j) = temp;
}

void sort(int* a, int size) // Implementation of bubblesort using pointer arithmetic
{
   int i, swapped = 1;
   while (swapped) // Run until there are no swaps left
   {
      swapped = 0;
      for (i=1; i<size; i++) // Iterate through the entire array
      {
         if (*(a+i-1) > *(a+i)) // Swap adjacent elements that are not in order
         {
            swap(a, i-1, i);
            swapped = 1;
         }
      }
   }
}

int main()
{
   int* input = (int*) malloc(10*sizeof(int)); // Create a test array
   int i;
   printf("Unsorted Array\n");
   for (i = 0; i < 10; i++) // Generate unsorted input
   {
      *(input + i) = rand() % 100 + 1;
      printf("%d\n", *(input + i));
   }
   sort(input, 10); // Sort it, in-place
   printf("Sorted Array\n");
   for (i = 0; i < 10; i++) // Print newly sorted array
   {
      printf("%d\n", *(input + i));
   }

   return 0;
}
