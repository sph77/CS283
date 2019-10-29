#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

volatile unsigned int counter = 0;
const int PEOPLE = 100; // Number of threads
const int GETUP = 1000; // Number of increments per thread

void *count(void *arg) // Increment thread
{
   int i;
   for (i = 0; i < GETUP; i++)
      counter++;
   return NULL;
}

int main()
{
   int i;
   pthread_t person[PEOPLE];
   for (i = 0; i < PEOPLE; i++) // Create all people threads
      pthread_create(&person[i], NULL, count, NULL);
   
   for (i = 0; i < PEOPLE; i++) // Wait on all threads
      pthread_join(person[i], NULL);

   if (counter != (unsigned)(PEOPLE * GETUP))
      printf("BOOM! counter = %d\n", counter); // Race condition
   else
      printf("OK    counter = %d\n", counter); // Counter OK
}
