#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "rsa.h"

// RSA key generator

int nth_prime(int n) // Get a random base and go n primes away from it
{
   int i = 0, j = 0;
   j = rand() % 101; // Random base from 1 to 100
   while (i < n)
   {
      j++;
      if (is_prime(j)) // Found a prime
      {
         i++;
      }
   }
   return j;
}

int main(int argc, char* argv[])
{
   srand(time(NULL)); // Get a random seed
   int mprime, nprime;
   rsa_t a, b, c, m, e, d;
   
   if (argc == 3) // Handle args if they exist
   {
      mprime = atoi(argv[1]);
      nprime = atoi(argv[2]);
   }
   else // Get args from user
   {
      printf("Enter the nth prime and the mth prime to compute\n");
      scanf("%d %d", &mprime, &nprime);
   }

   a = nth_prime(mprime);
   do
   {
      b = nth_prime(nprime);
   } while (b == a); // Different primes, loop if same (extremely rare)
   c = a * b;
   m = (a - 1) * (b - 1);
   while (1)
   {
      e = coprime(m); // Get a coprime to m which is coprime to c and is composite and e mod m > 1
      if (GCD(e, c) == 1 && !(is_prime(e)) && e % m > 1)
         break;
   }
   d = mod_inverse2(e, m);
   printf("%dth prime = %ld, %dth prime = %ld, c = %ld, m = %ld, e = %ld, d = %ld, Public Key = (%ld, %ld), Private Key = (%ld, %ld)\n", mprime, a, nprime, b, c, m, e, d, e, c, d, c);
   return 0;
}
