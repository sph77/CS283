#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "utilities.h"

// g1.c -- main entry point, query processing, route information to database library

database* db = NULL; // Global database struct

// SELECT * FROM TableName WHERE Field1="Value"
void select_cmd(char* name)
{
   char* field = NULL, * value = NULL;
   // On WHERE right now
   field = strtok(NULL, "="); // Field1
   value = strtok(NULL, "\"\n"); // Value
   database_select(db, name, field, value); // Route info to db select function
}

// DELETE FROM TableName WHERE Field1="Value"
void delete_cmd()
{
   char* name = NULL, * field = NULL, * value = NULL;
   strtok(NULL, " "); // FROM
   name = strtok(NULL, " "); // TableName
   strtok(NULL, " "); // WHERE
   field = strtok(NULL, "="); // Field1
   value = strtok(NULL, "\"\n"); // Value
   database_delete(db, name, field, value); // Route info to db delete function
}

// UPDATE TableName SET Field1="New value" WHERE Field2="Value"
void update_cmd()
{
   char* name = NULL, *field1 = NULL, *newval = NULL, *field2 = NULL, *oldval = NULL;
   name = strtok(NULL, " "); // TableName
   strtok(NULL, " "); // SET
   field1 = strtok(NULL, "="); // Field1
   newval = strtok(NULL, "\""); // New value
   strtok(NULL, " "); // WHERE
   field2 = strtok(NULL, "="); // Field 2
   oldval = strtok(NULL, "\"\n"); // Value
   database_update(db, name, field1, newval, field2, oldval); // Route info to db update function
}

// SELECT * FROM TableName JOIN TableName2 ON TableName.Field1=TableName.Field2
void join_cmd(char* name1)
{
   char* name1_copy = NULL, *field1 = NULL, *name2 = NULL, *name2_copy = NULL, *field2 = NULL; 
   name2 = strtok(NULL, " "); // TableName2
   strtok(NULL, " "); // ON
   name1_copy = strtok(NULL, " ."); // TableName (???)
   field1 = strtok(NULL, " .="); // Field1
   name2_copy = strtok(NULL, "= ."); // TableName (???)
   field2 = strtok(NULL, ". \n"); // Field2
   // TableName1.Field1 = TableName2.Field2
   // else flipped order, or invalid
   if ((safe_strcmp(name1, name1_copy) == 0) && (safe_strcmp(name2, name2_copy) == 0)) // Same order
      database_join(db, name1, field1, name2, field2);
   else if ((safe_strcmp(name2, name1_copy) == 0) && (safe_strcmp(name1, name2_copy) == 0)) // Flipped
      database_join(db, name1, field2, name2, field1);
}

// INSERT INTO TableName (Field1="Value1", Field2="Value2", ...)
void insert_cmd()
{
   char* name = NULL, * field = NULL, * value = NULL;
   strtok(NULL, " "); // INTO
   name = strtok(NULL, " "); // TableName
   database_add(db, name); // Create a new record for the table
   field = strtok(NULL, "(, =)\n");
   while (field != NULL) // Loop through field, values
   {
      value = strtok(NULL, "\"");
      database_insert(db, name, field, value); // Insert field, value into latest record
      field = strtok(NULL, "(, =)\n");
   }
}

// CREATE TABLE TableName FIELDS [Field1, Field2, ...]
void create_cmd()
{
   char* name = NULL, * field = NULL, ** fields = NULL;
   int i;
   int size = 0;
   int CAP_CHUNK = 10; // Num fields per chunk
   int cap = CAP_CHUNK;
   fields = (char**) safe_malloc(cap*sizeof(char*)); // Array of fields
   strtok(NULL, " "); // TABLE
   name = strtok(NULL, " "); // TableName
   strtok(NULL, " "); // FIELDS
   field = strtok(NULL, "[ ,]\n");
   while (field != NULL) // Loop through fields
   {
      *(fields+size) = NULL;
      safe_strcpy(fields+size, field); // Set field value
      field = strtok(NULL, "[ ,]\n");
      size++;
      if (size >= cap) // reallocate more fields
      {
         cap += CAP_CHUNK;
         fields = safe_realloc(fields, cap*sizeof(char*));
      }
   }
   table_construct(db, name, fields, size); // Add new table
   for (i = 0; i < size; i++) // Free fields array
      free(*(fields+i));
   free(fields);
}

// DROP TABLE TableName
void drop_cmd()
{
   char* name = NULL;
   strtok(NULL, " "); // TABLE
   name = strtok(NULL, " \n"); // TableName
   database_destroy(db, name); // Remove table from db
}

// Given query, figure out which of the seven comamnds to run
void process_query(char* query)
{
   char* tokens = NULL, * name = NULL;
   tokens = strtok(query, " "); // First word
   if (safe_strcmp(tokens, "SELECT") == 0)
   {
      strtok(NULL, " "); // *
      strtok(NULL, " "); // FROM

      name = strtok(NULL, " "); // TableName
      tokens = strtok(NULL, " "); // WHERE or JOIN
      if (safe_strcmp(tokens, "WHERE") == 0)
      {
         select_cmd(name);
      }
      else if (safe_strcmp(tokens, "JOIN") == 0)
      {
         join_cmd(name);
      }
   }
   else if (safe_strcmp(tokens, "DELETE") == 0)
   {
      delete_cmd();
   }
   else if (safe_strcmp(tokens, "UPDATE") == 0)
   {
      update_cmd();
   }
   else if (safe_strcmp(tokens, "INSERT") == 0)
   {
      insert_cmd();
   }
   else if (strcmp(tokens, "CREATE") == 0)
   {
      create_cmd();
   }
   else if (safe_strcmp(tokens, "DROP") == 0)
   {
      drop_cmd();
   }
}

// Loop and take user commands. Run respective database functions. On CTRL+D, write the database
// and free it.
int main(int argc, char *argv[])
{  
   int c;
   int CHUNK_SIZE = 4096; // Size of query buffer
   int capacity = CHUNK_SIZE;
   int size = 0;
   char* query = NULL;

   // Process args
   if (argc != 2) // Invalid arg count
   {
      fprintf(stderr, "(main) ERROR: Invalid arguments. Expected ./g1.c <DATABASE>\n");
      return 1;
   }
   else
   {
      db = database_open(argv[1]); // Open db
      if (db == NULL)
      {
         fprintf(stderr, "(main) ERROR: Unable to open db.\n");
         return 1;
      }
   }

   query = (char*) safe_malloc(sizeof(char)*CHUNK_SIZE);
   
   do // Query parse loop
   {
      printf("Enter query: "); // Get query from user
      size = 0;
      capacity = CHUNK_SIZE;
      // Adjust query buffer
      query = (char*) safe_realloc(query, sizeof(char)*CHUNK_SIZE);
      do
      {
         c = getchar(); // Parse char by char
         query[size] = (char)c; // Add char to buffer
         size++;
         if (size >= capacity) // Buffer is full
         {
            capacity += CHUNK_SIZE; // Allocate another chunk
            query = safe_realloc(query, capacity);
         }
      } while (c != '\n' && c != EOF); // Parse until newline or EOF
      query[size] = '\0'; // Terminate query
      process_query(query); // Run correct SQL command on db
   } while (c != EOF); // Ask for commands until EOF
   free(query);
   database_close(db); // Write db and free it
   printf("\n");
   return 0;
}
