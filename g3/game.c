#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "game.h"
#include "utilities.h"

board* board_create(int dim) // Create a board with NxN dimensions
{
   int i, j;
   board* b = safe_malloc(sizeof(board));
   b->dim = dim;
   b->grid = safe_malloc(dim * sizeof(char*));
   for (i = 0; i < dim; i++)
      b->grid[i] = safe_malloc(dim * sizeof(char));

   for (i = 0; i < dim; i++)
      for (j = 0; j < dim; j++)
         b->grid[i][j] = ' '; // Set every tile to blank
   return b;
}

void board_destroy(board* b) // Clean the board, free all pointers
{
   int i;
   for (i = 0; i < b->dim; i++)
      free(b->grid[i]);
   free(b->grid);
   free(b);
}

int board_isfull(board* b) // Check if entire top row is filled
{
   int i;
   for (i = 0; i < b->dim; i++)
   {
      if (b->grid[0][i] == ' ')
         return 0;
   }
   return 1;
}

int board_checkwin(board* b, char c, char d) // Check if four of a row (player c or player d) is filled. Return + for win, - for loss
{
   int row, col;
   // Check rows -
   for (row = 0; row < b->dim; row++)
   {
      for (col = 0; col < b->dim - 3; col++)
      {
         if (  (c == b->grid[row][col]) &&
               (c == b->grid[row][col+1]) &&
               (c == b->grid[row][col+2]) &&
               (c == b->grid[row][col+3])  )
            return 10000;
         else if (  (d == b->grid[row][col]) &&
               (d == b->grid[row][col+1]) &&
               (d == b->grid[row][col+2]) &&
               (d == b->grid[row][col+3])  )
            return -10000;
      }
   }
   // Check columns |
   for (row = 0; row < b->dim - 3; row++)
   {
      for (col = 0; col < b->dim; col++)
      {
         if (  (c == b->grid[row][col]) &&
               (c == b->grid[row+1][col]) &&
               (c == b->grid[row+2][col]) &&
               (c == b->grid[row+3][col])  )
            return 10000;
         else if (  (d == b->grid[row][col]) &&
               (d == b->grid[row+1][col]) &&
               (d == b->grid[row+2][col]) &&
               (d == b->grid[row+3][col])  )
            return -10000;
      }
   }
   // Check \ diagonals
   for (row = 0; row < b->dim - 3; row++)
   {
      for (col = 0; col < b->dim - 3; col++)
      {
         if (  (c == b->grid[row][col]) &&
               (c == b->grid[row+1][col+1]) &&
               (c == b->grid[row+2][col+2]) &&
               (c == b->grid[row+3][col+3])  )
            return 10000;
         else if (  (d == b->grid[row][col]) &&
               (d == b->grid[row+1][col+1]) &&
               (d == b->grid[row+2][col+2]) &&
               (d == b->grid[row+3][col+3])  )
            return -10000;
      }
   }
   // Check / diagonals
   for (row = 3; row < b->dim; row++)
   {
      for (col = 0; col < b->dim - 3; col++)
      {
         if (  (c == b->grid[row][col]) &&
               (c == b->grid[row-1][col+1]) &&
               (c == b->grid[row-2][col+2]) &&
               (c == b->grid[row-3][col+3])  )
            return 10000;
         else if (  (d == b->grid[row][col]) &&
               (d == b->grid[row-1][col+1]) &&
               (d == b->grid[row-2][col+2]) &&
               (d == b->grid[row-3][col+3])  )
            return -10000;
      }
   }
   return 0;
}

int board_score(board* b, char c, char d) // Give a prospective score to an uncomplete board
{
   // Method is very ugly but too lazy at this point to clean it up
   // +64 for three in a row
   // +8 for two in a row
   // +1 for one in a row
   // -64 for opponent three in a row
   // -8 for opponent two in a row
   // -1 for opponent one in a row
   // blank spaces in rows must be fillable (have 'ground' below), otherwise they aren't scored
   int row, col, score = 0;
   // Check rows -
   for (row = 0; row < b->dim; row++)
   {
      for (col = 0; col < b->dim - 3; col++)
      {
         if ((row != b->dim - 1) && (' ' == b->grid[row+1][col] || ' ' == b->grid[row+1][col+1] || ' ' == b->grid[row+1][col+2] || ' ' == b->grid[row+1][col+3]))
            continue;
         if    (  ((c == b->grid[row][col]) && (c == b->grid[row][col+1]) && (c == b->grid[row][col+2]) && (' ' == b->grid[row][col+3])) ||  
                  ((' ' == b->grid[row][col]) && (c == b->grid[row][col+1]) && (c == b->grid[row][col+2]) && (c == b->grid[row][col+3])) ||
                  ((c == b->grid[row][col]) && (' ' == b->grid[row][col+1]) && (c == b->grid[row][col+2]) && (c == b->grid[row][col+3])) ||
                  ((c == b->grid[row][col]) && (c == b->grid[row][col+1]) && (' ' == b->grid[row][col+2]) && (c == b->grid[row][col+3]))  )
            score += 64;
         else if  (  ((c == b->grid[row][col]) && (c == b->grid[row][col+1]) && (' ' == b->grid[row][col+2]) && (' ' == b->grid[row][col+3])) ||  
                     ((' ' == b->grid[row][col]) && (' ' == b->grid[row][col+1]) && (c == b->grid[row][col+2]) && (c == b->grid[row][col+3])) ||
                     ((' ' == b->grid[row][col]) && (c == b->grid[row][col+1]) && (c == b->grid[row][col+2]) && (' ' == b->grid[row][col+3])) || 
                     ((c == b->grid[row][col]) && (' ' == b->grid[row][col+1]) && (' ' == b->grid[row][col+2]) && (c == b->grid[row][col+3])) || 
                     ((c == b->grid[row][col]) && (' ' == b->grid[row][col+1]) && (c == b->grid[row][col+2]) && (' ' == b->grid[row][col+3])) || 
                     ((' ' == b->grid[row][col]) && (c == b->grid[row][col+1]) && (' ' == b->grid[row][col+2]) && (c == b->grid[row][col+3]))  ) 
            score += 8;
         else if  (  ((c == b->grid[row][col]) && (' ' == b->grid[row][col+1]) && (' ' == b->grid[row][col+2]) && (' ' == b->grid[row][col+3])) ||  
                     ((' ' == b->grid[row][col]) && (c == b->grid[row][col+1]) && (' ' == b->grid[row][col+2]) && (' ' == b->grid[row][col+3])) ||
                     ((' ' == b->grid[row][col]) && (' ' == b->grid[row][col+1]) && (c == b->grid[row][col+2]) && (' ' == b->grid[row][col+3])) ||
                     ((' ' == b->grid[row][col]) && (' ' == b->grid[row][col+1]) && (' ' == b->grid[row][col+2]) && (c == b->grid[row][col+3]))  ) 
            score += 1;
         else if  (  ((d == b->grid[row][col]) && (d == b->grid[row][col+1]) && (d == b->grid[row][col+2]) && (' ' == b->grid[row][col+3])) ||  
                     ((' ' == b->grid[row][col]) && (d == b->grid[row][col+1]) && (d == b->grid[row][col+2]) && (d == b->grid[row][col+3])) || 
                     ((d == b->grid[row][col]) && (' ' == b->grid[row][col+1]) && (d == b->grid[row][col+2]) && (d == b->grid[row][col+3])) ||
                     ((d == b->grid[row][col]) && (d == b->grid[row][col+1]) && (' ' == b->grid[row][col+2]) && (d == b->grid[row][col+3]))  )
            score -= 64;
         else if  (  ((d == b->grid[row][col]) && (d == b->grid[row][col+1]) && (' ' == b->grid[row][col+2]) && (' ' == b->grid[row][col+3])) ||  
                     ((' ' == b->grid[row][col]) && (' ' == b->grid[row][col+1]) && (d == b->grid[row][col+2]) && (d == b->grid[row][col+3])) ||
                     ((' ' == b->grid[row][col]) && (d == b->grid[row][col+1]) && (d == b->grid[row][col+2]) && (' ' == b->grid[row][col+3])) || 
                     ((d == b->grid[row][col]) && (' ' == b->grid[row][col+1]) && (' ' == b->grid[row][col+2]) && (d == b->grid[row][col+3])) || 
                     ((d == b->grid[row][col]) && (' ' == b->grid[row][col+1]) && (d == b->grid[row][col+2]) && (' ' == b->grid[row][col+3])) || 
                     ((' ' == b->grid[row][col]) && (d == b->grid[row][col+1]) && (' ' == b->grid[row][col+2]) && (d == b->grid[row][col+3]))  ) 
            score -= 8;
         else if  (  ((d == b->grid[row][col]) && (' ' == b->grid[row][col+1]) && (' ' == b->grid[row][col+2]) && (' ' == b->grid[row][col+3])) ||  
                     ((' ' == b->grid[row][col]) && (d == b->grid[row][col+1]) && (' ' == b->grid[row][col+2]) && (' ' == b->grid[row][col+3])) ||
                     ((' ' == b->grid[row][col]) && (' ' == b->grid[row][col+1]) && (d == b->grid[row][col+2]) && (' ' == b->grid[row][col+3])) ||
                     ((' ' == b->grid[row][col]) && (' ' == b->grid[row][col+1]) && (' ' == b->grid[row][col+2]) && (d == b->grid[row][col+3]))  ) 
            score -= 1;
      }
   }
   // Check columns | (note, no need to check ground)
   for (row = 0; row < b->dim - 3; row++)
   {
      for (col = 0; col < b->dim; col++)
      {
         if    (  ((' ' == b->grid[row][col]) && (c == b->grid[row+1][col]) && (c == b->grid[row+2][col]) && (c == b->grid[row+3][col])) ) 
            score += 64;
         else if  (  ((' ' == b->grid[row][col]) && (' ' == b->grid[row+1][col]) && (c == b->grid[row+2][col]) && (c == b->grid[row+3][col])) )
            score += 8;
         else if  (  ((' ' == b->grid[row][col]) && (' ' == b->grid[row+1][col]) && (' ' == b->grid[row+2][col]) && (c == b->grid[row+3][col])) ) 
            score += 1;
         else if  (  ((' ' == b->grid[row][col]) && (d == b->grid[row+1][col]) && (d == b->grid[row+2][col]) && (d == b->grid[row+3][col])) ) 
            score -= 64;
         else if  (  ((' ' == b->grid[row][col]) && (' ' == b->grid[row+1][col]) && (d == b->grid[row+2][col]) && (d == b->grid[row+3][col])) ) 
            score -= 8;
         else if  (  ((' ' == b->grid[row][col]) && (' ' == b->grid[row+1][col]) && (' ' == b->grid[row+2][col]) && (d == b->grid[row+3][col])) ) 
            score -= 1;
      }
   }
   // Check \ diagonals
   for (row = 0; row < b->dim - 3; row++)
   {
      for (col = 0; col < b->dim - 3; col++)
      {
         if (  ((row != b->dim - 4) && (' ' == b->grid[row+1][col] || ' ' == b->grid[row+2][col+1] || ' ' == b->grid[row+3][col+2] || ' ' == b->grid[row+4][col+3])) ||
               ((row == b->dim - 4) && (' ' == b->grid[row+1][col] || ' ' == b->grid[row+2][col+1] || ' ' == b->grid[row+3][col+2])) )
            continue;
         if    (  ((c == b->grid[row][col]) && (c == b->grid[row+1][col+1]) && (c == b->grid[row+2][col+2]) && (' ' == b->grid[row+3][col+3])) ||  
                  ((' ' == b->grid[row][col]) && (c == b->grid[row+1][col+1]) && (c == b->grid[row+2][col+2]) && (c == b->grid[row+3][col+3])) ||
                  ((c == b->grid[row][col]) && (' ' == b->grid[row+1][col+1]) && (c == b->grid[row+2][col+2]) && (c == b->grid[row+3][col+3])) ||
                  ((c == b->grid[row][col]) && (c == b->grid[row+1][col+1]) && (' ' == b->grid[row+2][col+2]) && (c == b->grid[row+3][col+3]))  ) 
            score += 64;
         else if  (  ((c == b->grid[row][col]) && (c == b->grid[row+1][col+1]) && (' ' == b->grid[row+2][col+2]) && (' ' == b->grid[row+3][col+3])) ||  
                     ((' ' == b->grid[row][col]) && (' ' == b->grid[row+1][col+1]) && (c == b->grid[row+2][col+2]) && (c == b->grid[row+3][col+3])) ||
                     ((' ' == b->grid[row][col]) && (c == b->grid[row+1][col+1]) && (c == b->grid[row+2][col+2]) && (' ' == b->grid[row+3][col+3])) || 
                     ((c == b->grid[row][col]) && (' ' == b->grid[row+1][col+1]) && (' ' == b->grid[row+2][col+2]) && (c == b->grid[row+3][col+3])) || 
                     ((c == b->grid[row][col]) && (' ' == b->grid[row+1][col+1]) && (c == b->grid[row+2][col+2]) && (' ' == b->grid[row+3][col+3])) || 
                     ((' ' == b->grid[row][col]) && (c == b->grid[row+1][col+1]) && (' ' == b->grid[row+2][col+2]) && (c == b->grid[row+3][col+3]))  ) 
            score += 8;
         else if  (  ((c == b->grid[row][col]) && (' ' == b->grid[row+1][col+1]) && (' ' == b->grid[row+2][col+2]) && (' ' == b->grid[row+3][col+3])) ||  
                     ((' ' == b->grid[row][col]) && (c == b->grid[row+1][col+1]) && (' ' == b->grid[row+2][col+2]) && (' ' == b->grid[row+3][col+3])) ||
                     ((' ' == b->grid[row][col]) && (' ' == b->grid[row+1][col+1]) && (c == b->grid[row+2][col+2]) && (' ' == b->grid[row+3][col+3])) ||
                     ((' ' == b->grid[row][col]) && (' ' == b->grid[row+1][col+1]) && (' ' == b->grid[row+2][col+2]) && (c == b->grid[row+3][col+3]))  ) 
            score += 1;
         else if  (  ((d == b->grid[row][col]) && (d == b->grid[row+1][col+1]) && (d == b->grid[row+2][col+2]) && (' ' == b->grid[row+3][col+3])) ||  
                     ((' ' == b->grid[row][col]) && (d == b->grid[row+1][col+1]) && (d == b->grid[row+2][col+2]) && (d == b->grid[row+3][col+3])) ||
                     ((d == b->grid[row][col]) && (' ' == b->grid[row+1][col+1]) && (d == b->grid[row+2][col+2]) && (d == b->grid[row+3][col+3])) ||
                     ((d == b->grid[row][col]) && (d == b->grid[row+1][col+1]) && (' ' == b->grid[row+2][col+2]) && (d == b->grid[row+3][col+3]))  )
            score -= 64;
         else if  (  ((d == b->grid[row][col]) && (d == b->grid[row+1][col+1]) && (' ' == b->grid[row+2][col+2]) && (' ' == b->grid[row+3][col+3])) ||  
                     ((' ' == b->grid[row][col]) && (' ' == b->grid[row+1][col+1]) && (d == b->grid[row+2][col+2]) && (d == b->grid[row+3][col+3])) ||
                     ((' ' == b->grid[row][col]) && (d == b->grid[row+1][col+1]) && (d == b->grid[row+2][col+2]) && (' ' == b->grid[row+3][col+3])) ||
                     ((d == b->grid[row][col]) && (' ' == b->grid[row+1][col+1]) && (' ' == b->grid[row+2][col+2]) && (d == b->grid[row+3][col+3])) || 
                     ((d == b->grid[row][col]) && (' ' == b->grid[row+1][col+1]) && (d == b->grid[row+2][col+2]) && (' ' == b->grid[row+3][col+3])) || 
                     ((' ' == b->grid[row][col]) && (d == b->grid[row+1][col+1]) && (' ' == b->grid[row+2][col+2]) && (d == b->grid[row+3][col+3]))  )
            score -= 8;
         else if  (  ((d == b->grid[row][col]) && (' ' == b->grid[row+1][col+1]) && (' ' == b->grid[row+2][col+2]) && (' ' == b->grid[row+3][col+3])) ||  
                     ((' ' == b->grid[row][col]) && (d == b->grid[row+1][col+1]) && (' ' == b->grid[row+2][col+2]) && (' ' == b->grid[row+3][col+3])) ||
                     ((' ' == b->grid[row][col]) && (' ' == b->grid[row+1][col+1]) && (d == b->grid[row+2][col+2]) && (' ' == b->grid[row+3][col+3])) ||
                     ((' ' == b->grid[row][col]) && (' ' == b->grid[row+1][col+1]) && (' ' == b->grid[row+2][col+2]) && (d == b->grid[row+3][col+3]))  ) 
            score -= 1;
      }
   }
   // Check / diagonals
   for (row = 3; row < b->dim; row++)
   {
      for (col = 0; col < b->dim - 3; col++)
      {
         if (  ((row != b->dim - 1) && (' ' == b->grid[row+1][col] || ' ' == b->grid[row][col+1] || ' ' == b->grid[row-1][col+2] || ' ' == b->grid[row-2][col+3])) ||
               ((row == b->dim - 1) && (' ' == b->grid[row][col+1] || ' ' == b->grid[row-2][col+1] || ' ' == b->grid[row-3][col+2])) )
            continue;
         if    (  ((c == b->grid[row][col]) && (c == b->grid[row-1][col+1]) && (c == b->grid[row-2][col+2]) && (' ' == b->grid[row-3][col+3])) ||  
                  ((' ' == b->grid[row][col]) && (c == b->grid[row-1][col+1]) && (c == b->grid[row-2][col+2]) && (c == b->grid[row-3][col+3])) ||
                  ((c == b->grid[row][col]) && (' ' == b->grid[row-1][col+1]) && (c == b->grid[row-2][col+2]) && (c == b->grid[row-3][col+3])) ||
                  ((c == b->grid[row][col]) && (c == b->grid[row-1][col+1]) && (' ' == b->grid[row-2][col+2]) && (c == b->grid[row-3][col+3]))  ) 
            score += 64;
         else if  (  ((c == b->grid[row][col]) && (c == b->grid[row-1][col+1]) && (' ' == b->grid[row-2][col+2]) && (' ' == b->grid[row-3][col+3])) ||  
                     ((' ' == b->grid[row][col]) && (' ' == b->grid[row-1][col+1]) && (c == b->grid[row-2][col+2]) && (c == b->grid[row-3][col+3])) ||
                     ((' ' == b->grid[row][col]) && (c == b->grid[row-1][col+1]) && (c == b->grid[row-2][col+2]) && (' ' == b->grid[row-3][col+3])) ||
                     ((c == b->grid[row][col]) && (' ' == b->grid[row-1][col+1]) && (' ' == b->grid[row-2][col+2]) && (c == b->grid[row-3][col+3])) || 
                     ((c == b->grid[row][col]) && (' ' == b->grid[row-1][col+1]) && (c == b->grid[row-2][col+2]) && (' ' == b->grid[row-3][col+3])) || 
                     ((' ' == b->grid[row][col]) && (c == b->grid[row-1][col+1]) && (' ' == b->grid[row-2][col+2]) && (c == b->grid[row-3][col+3]))  )
            score += 8;
         else if  (  ((c == b->grid[row][col]) && (' ' == b->grid[row-1][col+1]) && (' ' == b->grid[row-2][col+2]) && (' ' == b->grid[row-3][col+3])) ||  
                     ((' ' == b->grid[row][col]) && (c == b->grid[row-1][col+1]) && (' ' == b->grid[row-2][col+2]) && (' ' == b->grid[row-3][col+3])) ||
                     ((' ' == b->grid[row][col]) && (' ' == b->grid[row-1][col+1]) && (c == b->grid[row-2][col+2]) && (' ' == b->grid[row-3][col+3])) ||
                     ((' ' == b->grid[row][col]) && (' ' == b->grid[row-1][col+1]) && (' ' == b->grid[row-2][col+2]) && (c == b->grid[row-3][col+3]))  ) 
            score += 1;
         else if  (  ((d == b->grid[row][col]) && (d == b->grid[row-1][col+1]) && (d == b->grid[row-2][col+2]) && (' ' == b->grid[row-3][col+3])) ||  
                     ((' ' == b->grid[row][col]) && (d == b->grid[row-1][col+1]) && (d == b->grid[row-2][col+2]) && (d == b->grid[row-3][col+3])) || 
                     ((d == b->grid[row][col]) && (' ' == b->grid[row-1][col+1]) && (d == b->grid[row-2][col+2]) && (d == b->grid[row-3][col+3])) ||
                     ((d == b->grid[row][col]) && (d == b->grid[row-1][col+1]) && (' ' == b->grid[row-2][col+2]) && (d == b->grid[row-3][col+3]))  ) 
            score -= 64;
         else if  (  ((d == b->grid[row][col]) && (d == b->grid[row-1][col+1]) && (' ' == b->grid[row-2][col+2]) && (' ' == b->grid[row-3][col+3])) ||  
                     ((' ' == b->grid[row][col]) && (' ' == b->grid[row-1][col+1]) && (d == b->grid[row-2][col+2]) && (d == b->grid[row-3][col+3])) ||
                     ((' ' == b->grid[row][col]) && (d == b->grid[row-1][col+1]) && (d == b->grid[row-2][col+2]) && (' ' == b->grid[row-3][col+3])) ||
                     ((d == b->grid[row][col]) && (' ' == b->grid[row-1][col+1]) && (' ' == b->grid[row-2][col+2]) && (d == b->grid[row-3][col+3])) || 
                     ((d == b->grid[row][col]) && (' ' == b->grid[row-1][col+1]) && (d == b->grid[row-2][col+2]) && (' ' == b->grid[row-3][col+3])) || 
                     ((' ' == b->grid[row][col]) && (d == b->grid[row-1][col+1]) && (' ' == b->grid[row-2][col+2]) && (d == b->grid[row-3][col+3]))  )
            score -= 8;
         else if  (  ((d == b->grid[row][col]) && (' ' == b->grid[row-1][col+1]) && (' ' == b->grid[row-2][col+2]) && (' ' == b->grid[row-3][col+3])) ||  
                     ((' ' == b->grid[row][col]) && (d == b->grid[row-1][col+1]) && (' ' == b->grid[row-2][col+2]) && (' ' == b->grid[row-3][col+3])) ||
                     ((' ' == b->grid[row][col]) && (' ' == b->grid[row-1][col+1]) && (d == b->grid[row-2][col+2]) && (' ' == b->grid[row-3][col+3])) ||
                     ((' ' == b->grid[row][col]) && (' ' == b->grid[row-1][col+1]) && (' ' == b->grid[row-2][col+2]) && (d == b->grid[row-3][col+3]))  ) 
            score -= 1;
      }
   }
   return score;
}


void board_print(board* b) // Buffered print, to preserve between writes to stdout
{
   int p, i, j;
   char buf[((b->dim + 2) * (b->dim + 1))];
   for (p = 0; p < b->dim; p++)
      buf[p] = '-';
   buf[p] = '\n';
   for (i = 0; i < b->dim; i++) // Cells
   {
      for (j = 0; j < b->dim; j++)
      {
         p++;
         buf[p] = b->grid[i][j];
      }
      p++;
      buf[p] = '\n';
   }
   for (i = 0; i < b->dim; i++)
   {
      p++;
      buf[p] = '-';
   }
   p++;
   buf[p] = '\0'; // Necessary for printf
   printf("%s\n", buf);
}

move board_random(board* b) // Make a random move
{
   move m;
   int order[b->dim];
   int i, j, temp, row, col;
   for (i = 0; i < b->dim; i++) // Generate a list from 0 to N-1
      order[i] = i;
   for (i = b->dim - 1; i > 0; i--) // Create random permutation of that list
   {
      j = rand() % (i+1);
      temp = order[i];
      order[i] = order[j];
      order[j] = temp;
   }
   for (j = 0; j < b->dim; j++) // Go through the list, if a column is filled move to next column
   {
      for (i = b->dim - 1; i >= 0; i--)
      {
         row = i;
         col = order[j];
         if (b->grid[row][col] == ' ') // Free space, take it
         {
            m.row = row;
            m.col = col;
            return m;
         }
      }
   }
   m.row = -1;
   m.col = -1;
   return m; // Error return
}

int board_negamax(board* b, char c, char d, int mode, int depth) // Rough implementation of negamax algo
{
   int best, temp, col, row;
   int score = board_checkwin(b, c, d);
   if (score != 0) // We have a winner
      return mode*score;

   if (board_isfull(b)) // Tie game
      return 0;

   if (depth == 0) // Recursion limit
      return mode*board_score(b, c, d);

   best = -20000;
   for (col = 0; col < b->dim; col++) // Go through all potential next locations
   {
      for (row = b->dim - 1; row >= 0; row--)
      {
         if (b->grid[row][col] == ' ')
            break;
      }
      if (row < 0)
         continue;
      if (mode == 1) // Place the prospective turn
         b->grid[row][col] = c;
      else
         b->grid[row][col] = d;
   
      temp = -board_negamax(b, c, d, -mode, depth-1); // Get max of next player's move
      if (temp > best)
         best = temp;
      b->grid[row][col] = ' ';
   }
   return best;
}

move board_best(board* b, char c, char d) // Use negamax to determine the next best move
{
   move m;
   int best = -20000;
   int bestRow = -1, bestCol = -1;
   int col, row, score;
   for (col = 0; col < b->dim; col++) // Go through all prospective moves
   {
      for (row = b->dim - 1; row >= 0; row--)
      {
         if (b->grid[row][col] == ' ')
            break;
      }
      if (row < 0)
         continue;
      b->grid[row][col] = c;
      score = -board_negamax(b, c, d, -1, 2); // Run negamax with depth 2
      b->grid[row][col] = ' ';
      if (score > best) // Select the move with the best score
      {
         best = score;
         bestRow = row;
         bestCol = col;
      }
      else if (score == best && (rand() % 2) == 1) // Remove left bias
      {
         best = score;
         bestRow = row;
         bestCol = col;
      }
   }
   m.row = bestRow;
   m.col = bestCol;
   return m;
}

void board_place(board* b, move m, char c) // Place a move on the board
{
   if (m.row < 0 || m.row >= b->dim)
   {
      fprintf(stderr, "Error: row out of bounds\n");
      return;
   }
   if (m.col < 0 || m.col >= b->dim)
   {
      fprintf(stderr, "Error: col out of bounds\n");
      return;
   }
   if (b->grid[m.row][m.col] != ' ')
   {
      fprintf(stderr, "Error: location already filled\n");
      return;
   }
   b->grid[m.row][m.col] = c;
}
