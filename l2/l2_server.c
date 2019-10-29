#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include "csapp.h"
#include "utilities.h"

void send_response(int connfd) // Handle client connections
{ 
   struct stat statbuf;
   int status;
   int n = 0;
   long length = 0;
   char* buf = safe_malloc(MAXBUF);
   char* fpath = NULL;
   FILE *f = NULL;
   char* head = safe_malloc(MAXLINE);
   char* body = NULL;
   rio_t rio;

   Rio_readinitb(&rio, connfd);
   while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0)
   {
      if (strncmp(buf, "GET /", 5) == 0) // Found it
      {
         strtok(buf, " "); // GET
         fpath = strtok(NULL, " "); // /path (relative)
         // No absolute paths (starting at /) allowed
         // No relative paths including . or .. allowed
         if (  (strncmp(fpath, "//", 2) == 0) || // Absolute path
               (strcmp(fpath, "/..") == 0) || // .. at start
               (strstr(fpath, "/../") != NULL) || // /../ in middle
               (strcmp(fpath+strlen(fpath)-3, "/..") == 0) || // /.. at end
               (strcmp(fpath, "/.") == 0) || // . at start
               (strstr(fpath, "/./") != NULL) || // /./ in middle
               (strcmp(fpath+strlen(fpath)-2, "/.") == 0) // /. at end
            )
         {
            strcpy(head, "HTTP/1.1 403 Forbidden\r\nContent-Length: 0\r\n\r\n");
            Rio_writen(connfd, head, strlen(head)); // Send up to (but not including) \0
            free(buf);
            free(head);
            return;
         }

         if (strcmp(fpath, "/") == 0)
            fpath = "/index.html";

         fpath++; // Ignore /

         status = stat(fpath, &statbuf);
         if (status != 0) // File doesn't exist
         {
            strcpy(head, "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n");
            Rio_writen(connfd, head, strlen(head));
            free(buf);
            free(head);
            return;
         }
         if (S_ISDIR(statbuf.st_mode)) // Directory, default on index.html
         {
            if (*(fpath+strlen(fpath)-1) != '/') // Check if it is dir/ or dir
               strcat(fpath, "/");
            strcat(fpath, "index.html");
            // Get size of file (try #2)
            status = stat(fpath, &statbuf);
            if (status != 0)
            {
               strcpy(head, "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n");
               Rio_writen(connfd, head, strlen(head));
               free(buf);
               free(head);
               return;
            }
         }
         if (!S_ISREG(statbuf.st_mode)) // Not a file
         {
            strcpy(head, "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n");
            Rio_writen(connfd, head, strlen(head));
            free(buf);
            free(head);
            return;
         }
         // If we got here, then fpath must be a legitimate file

         length = statbuf.st_size; // Number of bytes
         // Header
         sprintf(head, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n", length);
         Rio_writen(connfd, head, strlen(head)); // Send header (not including \0)
         if (length != 0) // Open file and get body if it is valid
         {
            f = fopen(fpath, "rb");
            body = safe_malloc(length);
            fread(body, 1, length, f);
            fclose(f);
            Rio_writen(connfd, body, length); // Send body (not including \0 or EOF)
            free(body);
         }
         free(buf);
         free(head);
         return;
      }
      else if (strcmp(buf, "\r\n") == 0) // Closing header, something went wrong
      {
         strcpy(head, "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n");
         Rio_writen(connfd, head, strlen(head));
         free(buf);
         free(head);
         return;
      }
   }
}

int main(int argc, char* argv[])
{
   int c, listenfd, connfd;
   unsigned int clientlen;
   struct sockaddr_in clientaddr;
   int port = -1;

   while ((c = getopt(argc, argv, "p:")) != -1)
   {
      switch(c)
      {
         case 'p':
            port = atoi(optarg);
            break;
      }
   }

   if (port < 0)
   {
      fprintf(stderr, "(main) ERROR: Port not specified\n");
      return 1;
   }

   listenfd = Open_listenfd(port);

   while (1)
   {
      clientlen = sizeof(clientaddr);
      connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
      send_response(connfd);
      Close(connfd);
   }

   Close(listenfd);
   return 0;
}
