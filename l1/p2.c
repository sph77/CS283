#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void p2()
{
   char** words = (char**) malloc(10*sizeof(char*)); // Allocate an array of 10 strings
   int i;
   for (i=0; i<10; i++) // Allocate each string to 15 chars
   {
      words[i] = (char*) malloc(15*sizeof(char));
   }
   // Assign all of the strings, using strcpy
   // Null terminator \0 is not required in string itself
   // See - https://linux.die.net/man/3/strcpy
   strcpy(words[0], "Lorem");
   strcpy(words[1], "ipsum");
   strcpy(words[2], "dolor");
   strcpy(words[3], "sit");
   strcpy(words[4], "amet,");
   strcpy(words[5], "consectetur");
   strcpy(words[6], "adipiscing");
   strcpy(words[7], "alit,");
   strcpy(words[8], "sed");
   strcpy(words[9], "do");
   for (i=0; i<10; i++) // Print all 10 strings out
   {
      printf("words[%d] = %s\n", i, words[i]);
   }
   for (i=0; i<10; i++) // Free the individual strings first
   {
      free(words[i]);
   }
   free(words); // Now free the array of strings
}

int main()
{
   p2();
   return 0;
}
