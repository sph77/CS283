#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "rsa.h"

#define MAX 4096

// RSA key cracker

void next_prime(long* start) // Update value at start to next prime number
{
   while (1)
   {
      (*start)++;
      if (is_prime(*start)) // Found a prime
      {
         break;
      }
   }
}

int main(int argc, char* argv[])
{
   int verbose = 0;
   rsa_t msg, ch;
   rsa_t a = 2, b = 2, c, e, m, d;
   char buf[MAX];

   while ((c = getopt(argc, argv, "v")) != -1) // Process verbose arg, if exists
   {
      switch(c)
      {
         case 'v':
            verbose = 1;
            break;
      }
   }
   
   if (argv[optind] != NULL && argv[optind + 1] != NULL) // Get e, c as args
   {
      e = strtol(argv[optind], NULL, 10);
      c = strtol(argv[optind+1], NULL, 10);
   }
   else // User input e, c
   {
      printf("Enter the public key value\n");
      scanf("%ld %ld", &e, &c);
   }

   while (1) // Assuming a, b are prime
   {
      if (verbose) // Verbose mode
         printf("Testing a = %ld\n", a);
      if (c % a == 0) // c has only ONE unique prime factorization, a * b
      {
         b = c / a;
         break; // We found a, b
      }
      else
      {
         next_prime(&a); // Try next prime
      }
   }

   m = (a - 1) * (b - 1);
   d = mod_inverse2(e, m);
   printf("a was %ld b was %ld\n", a, b);
   printf("m is %ld\n", m);
   printf("D was found to be %ld\n", d);

   do // Decryption loop
   {
      printf("Enter next char cipher value as an int, type quit to quit\n");
      scanf("%s", buf);
      if (strcmp(buf, "quit") != 0) // Check if quit
      {
         msg = strtol(buf, NULL, 10);
         ch = endecrypt(msg, d, c);
         printf("This char decrypted to %ld\n", ch);
         printf("The letter is %c\n", (char)ch);
      }
   } while (strcmp(buf, "quit") != 0);
   
   return 0;
}
