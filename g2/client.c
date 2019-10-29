#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include "csapp.h"
#include "rsa.h"
#include "utilities.h"

// Globals, for connections and key values
rsa_t e = 0, c = 0, d = 0, dc = 0;
rio_t rio;
int clientfd;
int terminate = 0;

void *g2_read(void *vargp) // Read from socket thread
{
   pthread_detach(pthread_self()); // Make not-joinable
   rsa_t cipher;
   char buf[MAXBUF];
   int offset, active = 1, n;
   while (active) // While the received message is not terminating
   {
      offset = 0;
      memset(buf, '\0', MAXBUF); // Clean buffer
      do
      {
         n = rio_readnb(&rio, &cipher, sizeof(rsa_t)); // Read a char
         if (n == 0) // Cannot read
            break;
         if (cipher == 0) // Terminating cipher
            break;
         buf[offset] = (char)endecrypt(cipher, d, dc); // Decrypt it
         offset++; // Next char
      } while (buf[offset-1] != '\0'); // String is terminated
      if (n == 0) // Nothing left to read
         break;
      if (strcmp(buf, ".bye") == 0) // Terminating message received
         active = 0;
      fputs(buf, stdout);
      fputs("\n", stdout);
   }
   terminate = 1;
   return NULL;
}

void *g2_write(void *vargp) // Write to socket thread
{
   pthread_detach(pthread_self()); // Make not-joinable
   int i, active = 1;
   char buf[MAXBUF];
   rsa_t msg[MAXBUF];
   while (active) // While terminating message not sent
   {
      memset(buf, '\0', MAXBUF); // Clean buffer
      memset(msg, 0, sizeof(rsa_t)*MAXBUF); // Clean cipher msg
      printf("Type, enter .bye to quit\n");
      fgets(buf, MAXBUF, stdin); // Get input
      buf[strlen(buf)-1] = '\0'; // Change \n to \0
      printf("Encrypted %s to", buf);
      if (strcmp(buf, ".bye") == 0) // Terminating message sent
         active = 0;
      for (i = 0; i < strlen(buf); i++) // Encrypt every char
      {
         msg[i] = endecrypt(buf[i], e, c);
         printf(" %ld", msg[i]);
      }
      printf("\n");
      rio_writen(clientfd, msg, (strlen(buf)+1)*sizeof(rsa_t)); // Send message (silently)
   }
   terminate = 2;
   return NULL;
}

int main(int argc, char* argv[])
{
   pthread_t read_thread, write_thread;
   int port = -1;
   int ch;
   char* server = NULL;
   char* ptr = NULL;

   while ((ch = getopt(argc, argv, "s:p:e:d:")) != -1) // Process args
   {
      switch(ch)
      {
         case 's':
            safe_strcpy(&server, optarg);
            break;
         case 'p':
            port = atoi(optarg);
            break;
         case 'e':
            e = strtol(optarg, &ptr, 10);
            c = strtol(ptr, NULL, 10);
            break;
         case 'd':
            d = strtol(optarg, &ptr, 10);
            dc = strtol(ptr, NULL, 10);
            break;
      }
   }

   // Validate args
   if (server == NULL)
   {
      fprintf(stderr, "(main) ERROR: Server not specified\n");
      return 1;
   }
   else if (port < 0)
   {
      fprintf(stderr, "(main) ERROR: Port not specified\n");
      free(server);
      return 1;
   }
   else if (e == 0 || c == 0 || d == 0 || dc == 0)
   {
      fprintf(stderr, "(main) ERROR: Missing key data\n");
      free(server);
      return 1;
   }

   printf("Binding to port %d, please wait...\n", port);

   clientfd = open_clientfd(server, port); // Bind to port (silently)
   if (clientfd < 0)
   {
      fprintf(stderr, "(main) ERROR: Unable to bind to port\n");
      free(server);
      return 1;
   }
   Rio_readinitb(&rio, clientfd);

   printf("Client started\n");
   if (pthread_create(&read_thread, NULL, g2_read, NULL)) // Read thread
   {
      fprintf(stderr, "Error creating read thread\n");
      Close(clientfd);
      free(server);
      exit(1);
   }
   if (pthread_create(&write_thread, NULL, g2_write, NULL)) // Write thread
   {
      fprintf(stderr, "Error creating write thread\n");
      Close(clientfd);
      free(server);
      exit(1);
   }

   while (terminate == 0) // Wait for either thread to terminate
   {
   }

   Close(clientfd);
   free(server);
   return 0;
}
