#ifndef rsa_H_
#define rsa_H_

typedef long rsa_t; // Type for key

// Get rand comprime num to x
rsa_t coprime(rsa_t x);
// Given message/cipher, encrypt or decrypt it
rsa_t endecrypt(rsa_t msg_or_cipher, rsa_t key, rsa_t c);
// Get the GCD of a and b
rsa_t GCD(rsa_t a, rsa_t b);
// Calculate the inverse for base and m
rsa_t mod_inverse(rsa_t base, rsa_t m);
// Calculate the inverse for base and m (alternate algo)
rsa_t mod_inverse2(rsa_t base, rsa_t m);
// Calculate (a^b) mod c
rsa_t modulo(rsa_t a, rsa_t b, rsa_t c);
// Calculate Euler's totient for n
rsa_t totient(rsa_t n);
// Determine if n is prime
int is_prime(rsa_t n);

#endif
