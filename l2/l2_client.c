#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <getopt.h>
#include "csapp.h"
#include "utilities.h"

int main(int argc, char* argv[])
{
   int c, clientfd, n, chunked = 0;
   unsigned int content_length = 0;
   int port = 80;
   char* host = NULL, *file = NULL;
   char* buf = safe_malloc(MAXBUF);
   rio_t rio;

   while ((c = getopt(argc, argv, "f:h:p:")) != -1) // Load in getopt args
   {
      switch(c)
      {
         case 'f':
            file = safe_malloc(strlen(optarg)+1);
            strcpy(file, optarg);
            break;
         case 'h':
            host = safe_malloc(strlen(optarg)+1);
            strcpy(host, optarg);
            break;
         case 'p':
            port = atoi(optarg);
            break;
      }
   }

   if (file == NULL) // File required
   {
      fprintf(stderr, "(main) ERROR: File not specified\n");
      free(buf);
      if (host != NULL)
         free(host);
      return 1;
   }

   if (host == NULL) // Host required
   {
      fprintf(stderr, "(main) ERROR: Host not specified\n");
      free(buf);
      if (file != NULL)
         free(file);
      return 1;
   }

   clientfd = Open_clientfd(host, port); // Create connection to server

   Rio_readinitb(&rio, clientfd);
   sprintf(buf, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", file, host); // Send GET header
   Rio_writen(clientfd, buf, strlen(buf));
   //shutdown(clientfd, SHUT_WR);
   while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) // Read headers
   {
      fputs(buf, stdout);
      if (strncmp(buf, "Content-Length: ", 16) == 0) // Found length of body
      {
         sscanf(buf, "Content-Length: %d\r\n", &content_length);
      }
      else if (strcmp(buf, "Transfer-Encoding: chunked\r\n") == 0) // Body uses chunked encoding
      {
         chunked = 1;
      }
      else if (strcmp(buf, "\r\n") == 0) // Done with headers
      {
         break;
      }
      memset(buf, '\0', MAXBUF);
   }

   if (content_length != 0) // Get the body if it exists
   {
      buf = safe_realloc(buf, content_length+1);
      n = Rio_readnb(&rio, buf, content_length);
      *(buf+content_length) = '\0'; // Terminate it
      fputs(buf, stdout);
   }
   else if (chunked) // Handle chunked encoding
   {
      do
      {
         buf = safe_realloc(buf, MAXLINE);
         memset(buf, '\0', MAXLINE);
         n = Rio_readlineb(&rio, buf, MAXLINE);
         sscanf(buf, "%x\r\n", &content_length); // Content-Length is length of data in hex
         buf = safe_realloc(buf, content_length+2); // Include \r\n
         n = Rio_readnb(&rio, buf, content_length+2); // Include \r\n
         *(buf+content_length) = '\0'; // Terminate it
         fputs(buf, stdout);
      } while (content_length != 0);
   }

   Close(clientfd); // Close connection
   free(buf);
   free(host);
   free(file);
   printf("\n");
   return 0;
}
