#include <stdlib.h>
#include <stdio.h>

void p1()
{
   int* vals = (int*) malloc(10*sizeof(int)); // Allocate 10 ints for int pointer
   int i, sum = 0;
   for (i=0; i<10; i++) // Assign all 10 ints to values
   {
      sum += i;
      vals[i] = sum;
   }
   for (i=0; i<10; i++) // Print all 10 values
   {
      printf("vals[%d] = %d\n", i, vals[i]);
   }
   free(vals); // Free the integer array
}

int main()
{
   p1();
   return 0;
}
