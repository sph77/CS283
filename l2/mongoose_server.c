// Ripped and ganked from cesanta.com/docs/http/server-example.html

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include "mongoose.h"
#include "utilities.h"

#define MAXBUF 4096

static char* s_http_port = "40001"; // Default port num

static void ev_handler(struct mg_connection *c, int ev, void *p) // Send response to client
{
   struct http_message *hm = NULL; // Request message
   struct stat statbuf;
   int status;
   long length = 0;
   char* fpath = NULL;
   FILE *f = NULL;
   char* body = NULL;
   if (ev == MG_EV_HTTP_REQUEST) // HTTP only
   {
      hm = (struct http_message *) p; // Cast message to http message
      if (  (strncmp(hm->method.p, "GET", 3) != 0) 
            || (strncmp(hm->proto.p, "HTTP/1.1", 8) != 0)
         )// Only support GET requests for HTTP/1.1
      {
         mg_send_head(c, 403, 0, NULL); // Unsupported request
         return;
      }

      fpath = safe_malloc(hm->uri.len+1); // Allocate and copy filepath
      strncpy(fpath, hm->uri.p, hm->uri.len+1);
      *(fpath+hm->uri.len) = '\0'; // Terminate fpath
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
         mg_send_head(c, 403, 0, NULL);
         free(fpath);
         return;
      }

      if (strcmp(fpath, "/") == 0)
         strcpy(fpath, "/index.html");

      // Get size of file
      status = stat(fpath+1, &statbuf);
      if (status != 0) // File doesn't exist
      {
         mg_send_head(c, 404, 0, NULL);
         free(fpath);
         return;
      }

      if (S_ISDIR(statbuf.st_mode)) // Directory, default on index.html
      {
         if (*(fpath+strlen(fpath)-1) != '/') // Check if it is dir/ or dir
            strcat(fpath, "/");
         strcat(fpath, "index.html");
         // Get size of file (try #2)
         status = stat(fpath+1, &statbuf);
         if (status != 0)
         {
            mg_send_head(c, 404, 0, NULL);
            free(fpath);
            return;
         }
      }
      if (!S_ISREG(statbuf.st_mode)) // Not a file
      {
         mg_send_head(c, 404, 0, NULL);
         free(fpath);
         return;
      }
      // If we got here, then fpath must be a legitimate file

      length = statbuf.st_size; // Number of bytes
      // Header
      mg_send_head(c, 200, length, NULL);
      if (length != 0) // Send body of file if exists
      {
         // Read the file
         f = fopen(fpath+1, "rb");
         body = safe_malloc(length);
         fread(body, 1, length, f);
         fclose(f);
         mg_send(c, body, length);
         free(body);
      }
      free(fpath);
   }
}

int main(int argc, char* argv[])
{
   struct mg_mgr mgr;
   struct mg_connection *conn;
   int c;
   int port = -1;

   while ((c = getopt(argc, argv, "p:")) != -1) // Handle getopt args
   {
      switch(c)
      {
         case 'p':
            s_http_port = optarg;
            port = atoi(optarg);
            break;
      }
   }

   if (port < 0)
   {
      fprintf(stderr, "(main) ERROR: Port not specified\n");
      return 1;
   }

   mg_mgr_init(&mgr, NULL); // Initialize mongoose manager
   conn = mg_bind(&mgr, s_http_port, ev_handler); // Bind http port to http event
   mg_set_protocol_http_websocket(conn); // Make the connection a http websocket

   for (;;) // Poll indefinitely for requests
   {
      mg_mgr_poll(&mgr, 1000); 
   }

   mg_mgr_free(&mgr);
   return 0;
}
