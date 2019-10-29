#include <math.h>
#include <time.h>
#include <stdlib.h>

#include "rsa.h"

// RSA library

rsa_t coprime(rsa_t x) // Get a random number coprime to x
{
   rsa_t r;
   // See https://stackoverflow.com/questions/822323/how-to-generate-a-random-int-in-c/39475626#39475626
   srand(time(NULL)); // Get a random seed
   while (1)
   {
      r = (rand() % (4*x - 2*x + 1)) + 2*x; // Random target in range from 2*x to 4*x
      if (GCD(r, x) == 1) // Found a coprime number
         break;
   }
   return (rsa_t)r;
}

rsa_t endecrypt(rsa_t msg_or_cipher, rsa_t key, rsa_t c) // Encrypt/decrypt character
{
   return modulo(msg_or_cipher, key, c); // Just a wrapper for modulo
}

rsa_t GCD(rsa_t a, rsa_t b) // Get the GCD of a and b
{
   rsa_t t;
   // en.wikipedia.org/wiki/Euclidean_algorithm
   while (b != 0) // GCD of a and b is same as GCD of b and remainder
   {
      t = b;
      b = a % b;
      a = t;
   }
   return a;
}

rsa_t mod_inverse(rsa_t base, rsa_t m) // Get the inverse of base and m
{
   rsa_t quotient, temp;
   rsa_t t = 0, newt = 1, r = m, newr = base;
   // en.wikipedia.org/wiki/Extended_Euclidean_algorithm
   while (newr != 0) // Use Euclidean algorithm to work backwards to find inverse
   {
      quotient = r / newr;
      temp = newt;
      newt = t - quotient * newt;
      t = temp;
      temp = newr;
      newr = r - quotient * newr;
      r = temp;
   }
   if (r > 1)
      return -1;
   if (t < 0)
      t = t + m;
   return (rsa_t)t;
}

rsa_t mod_inverse2(rsa_t base, rsa_t m) // Alternate algo for mod inverse
{
   return modulo(base, totient(m) - 1, m); // Same as (base ^ (totient(m) - 1)) mod m
}

rsa_t modulo(rsa_t a, rsa_t b, rsa_t c) // Calc (a^b) mod m
{
   rsa_t y, i, k, B;
   // homepages.math.uic.edu/~leon/cs-mcs401-s08/handouts/fastexp.pdf
   if (b == 0)
      return 1;

   y = a;
   k = floor(log2(b)); // Takes O(log(n)) instead of O(n)
   for (i = k-1; i >= 0; i--) // Iterate down the log
   {
      B = (b >> i) & 1; // Bitwise operator to determine even/odd power
      if (B == 0) // Even power
         y = (y*y) % c;
      else // Odd power, include additional a
         y = (y*y*a) % c;
   }

   return y;
}

rsa_t totient(rsa_t n) // Calculate totient of n
{
   // Using Euler's product method for faster computation
   // www.geeksforgeeks.org/eulers-totient-function/
 
   rsa_t t = n;
   rsa_t i;
   for (i = 2; i*i <= n; ++i) // Handle from 2 to sqrt(n) (greatest prime divisor <= sqrt(n))
   {
      if (n % i == 0) // If even divides, remove all combinations of divisor from totient
      {
         while (n % i == 0) // Reduce n down (do not double count factors)
            n /= i;
         t -= t / i; // Remove from totient
      }
   }
   if (n > 1)
      t -= t / n;
   return t;
}

int is_prime(rsa_t n) // Check if n is prime
{
   int i;
   // en.wikipedia.org/wiki/Primality_test
   if (n <= 3) // 2, 3 prime
      return (n > 1);
   else if ((n % 2 == 0) || (n % 3 == 0)) // Do not count multiples of 2, 3
      return 0;

   i = 5; // 5 mod 6 = -1
   while (i * i <= n) // Greatest prime divisor <= sqrt(n), so only test up to there
   {
      // n mod (6*x - 1) || n mod (6*x + 1)
      if ((n % i == 0) || (n % (i + 2) == 0)) // Abusing the fact that n mod 6 is either -1 or 1 if n is prime
         return 0; // Not prime, some lower item divides it
      i += 6; // Move up 6
   }
   return 1; // n is prime, tested all possible divisors
}
