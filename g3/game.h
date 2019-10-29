#include <unistd.h>
#include <sys/types.h>

typedef struct move
{
   int row;
   int col;
} move;

typedef struct board 
{
   int dim;
   char** grid;
} board;

typedef struct game
{
   board* b;
   int done;
   int pipefd[2][2];
   pid_t pid;
   int id;
} game;

board* board_create(int dim);
void board_destroy(board* b);
int board_isfull(board* b);
int board_checkwin(board* b, char c, char d);
int board_score(board* b, char c, char d);
void board_print(board* b);
move board_random(board* b);
int board_negamax(board* b, char c, char d, int mode, int depth);
move board_best(board* b, char c, char d);
void board_place(board* b, move m, char c);
