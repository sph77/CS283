#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "game.h"
#include "utilities.h"

void child_process(game g) // Game loop for child
{
   move* m;
   char buf[sizeof(move)];
   m = (move*)buf; // Point m on buf
   while (read(g.pipefd[0][0], &buf, sizeof(move)) > 0) // Get parent move
   {
      board_place(g.b, *m, 'X'); // Update child board to reflect parent
      if (board_checkwin(g.b, 'X', 'O') != 0 || board_isfull(g.b)) // Parent won or tied
         break;
      *m = board_random(g.b); // Note that this modifies buf
      board_place(g.b, *m, 'O'); // Place random move
      if (write(g.pipefd[1][1], &buf, sizeof(move)) < 0) // Send move to parent
      {
         fprintf(stderr, "Error in writing from child\n");
         exit(1);
      }
      if (board_checkwin(g.b, 'O', 'X') > 0) // On win, end game
      {
         break;
      }
      else if (board_isfull(g.b)) // On tie, end game
      {
         break;
      }
   }
   close(g.pipefd[0][0]);
   close(g.pipefd[1][1]);
}

int main(int argc, char *argv[])
{
   srand(time(NULL)); // Set parent random
   int i, j;
   move* m; // Move buffer
   char buf[sizeof(move)];
   pid_t wpid;
   int child_status;
   int ngames = 0, dimensions = 0, finished = 0;
   game* games; // List of all games
   if (argc != 3)
   {
      fprintf(stderr, "Improper number of arguments. Expected: 'g3.c <GAMES> <DIMENSIONS>'\n");
      exit(0);
   }
   else
   {
      ngames = atoi(argv[1]);
      dimensions = atoi(argv[2]);
   }

   if (ngames <= 0 || dimensions < 4)
   {
      fprintf(stderr, "Invalid arguments.\n");
      exit(0);
   }

   games = safe_malloc(ngames*sizeof(game)); // Create N games

   for (i = 0; i < ngames; i++) // Initialize all games
   {
      games[i].id = i + 1;
      games[i].b = board_create(dimensions); // Blank board
      games[i].done = 0;
      games[i].pid = 0; // Default pid
      if (pipe(games[i].pipefd[0]) < 0)
      {
         fprintf(stderr, "Unable to create write pipe for parent\n");
         exit(1);
      }
      if (pipe(games[i].pipefd[1]) < 0)
      {
         fprintf(stderr, "Unable to create write pipe for child\n");
         exit(1);
      }
   }

   for (i = 0; i < ngames; i++) // Set up processes
   {
      games[i].pid = fork(); // Create child process
      if (games[i].pid < 0)
      {
         fprintf(stderr, "Unable to create child\n");
         exit(1);
      }
      else if (games[i].pid == 0) // Child
      {
         srand(games[i].pid*time(NULL)); // Child random
         close(games[i].pipefd[0][1]); // Close write end of pipe1
         close(games[i].pipefd[1][0]); // Close read end of pipe2
         child_process(games[i]); // Run child process
         for (i = 0; i < ngames; i++)
            board_destroy(games[i].b);
         free(games);
         exit(0); // Exit gracefully
      }
      else // Parent
      {
         close(games[i].pipefd[0][0]); // Close read end of pipe1
         close(games[i].pipefd[1][1]); // Close write end of pipe2
      }
   }

   for (i = 0; i < ngames; i++) // Send the first move to every game
   {
      m = (move*)buf; // Point m onto buf
      *m = board_best(games[i].b, 'X', 'O'); // This modifies buf
      board_place(games[i].b, *m, 'X'); // Place best move for parent
      printf("GAME %d: Parent placed X at position %d, %d\n", games[i].id, m->row, m->col);
      board_print(games[i].b);
      if (write(games[i].pipefd[0][1], &buf, sizeof(move)) < 0) // Send move to child
      {
         fprintf(stderr, "Error in writing from parent\n");
         exit(1);
      }
   }

   // Do the game here
   while (finished < ngames) // Keep all print statements in parent (synchronized prints)
   {
      for (i = 0; i < ngames; i++)
      {
         if (games[i].done) // Skip game, it's already done
            continue;
         if (read(games[i].pipefd[1][0], &buf, sizeof(move)) < 0) // Nothing to receive
         {
            finished++;
            games[i].done = 1;
            close(games[i].pipefd[1][0]);
            close(games[i].pipefd[0][1]);
            continue;
         }
         board_place(games[i].b, *m, 'O'); // Copy child move onto parent board
         printf("GAME %d: Child placed O at position %d, %d\n", games[i].id, m->row, m->col);
         board_print(games[i].b);
         if (board_checkwin(games[i].b, 'O', 'X') > 0) // Child win
         {
            printf("GAME %d: Child wins!\n", games[i].id);
            finished++;
            games[i].done = 1;
            close(games[i].pipefd[1][0]);
            close(games[i].pipefd[0][1]);
            continue;
         }
         else if (board_isfull(games[i].b)) // Child tied game
         {
            printf("GAME %d: Game over, board is full.\n", games[i].id);
            finished++;
            games[i].done = 1;
            close(games[i].pipefd[1][0]);
            close(games[i].pipefd[0][1]);
            continue;
         }
         *m = board_best(games[i].b, 'X', 'O'); // Note that this modifies buf
         board_place(games[i].b, *m, 'X'); // Place best move
         printf("GAME %d: Parent placed X at position %d, %d\n", games[i].id, m->row, m->col);
         board_print(games[i].b);
         if (write(games[i].pipefd[0][1], &buf, sizeof(move)) < 0) // Send move to child
         {
            fprintf(stderr, "Error in writing from parent\n");
            exit(1);
         }
         if (board_checkwin(games[i].b, 'X', 'O') > 0) // Check if parent wins
         {
            printf("GAME %d: Parent wins!\n", games[i].id);
            finished++;
            games[i].done = 1;
            close(games[i].pipefd[1][0]);
            close(games[i].pipefd[0][1]);
            continue;
         }
         else if (board_isfull(games[i].b)) // Check if parent tied game
         {
            printf("GAME %d: Game over, board is full.\n", games[i].id);
            finished++;
            games[i].done = 1;
            close(games[i].pipefd[1][0]);
            close(games[i].pipefd[0][1]);
            continue;
         }
      }
   }

   // Wait until Anakin kills the younglings (all processes die)
   for (i = 0; i < ngames; i++)
   {
      wpid = waitpid(games[i].pid, &child_status, 0);
      if (WIFEXITED(child_status))
         printf("Child %d terminated with exit status %d\n", wpid, WEXITSTATUS(child_status));
      else
         fprintf(stderr, "Child %d terminated abnormally\n", wpid);
   }

   // Free everything
   for (i = 0; i < ngames; i++)
      board_destroy(games[i].b);
   free(games);
   return 0;
}
