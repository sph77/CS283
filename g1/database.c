#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "database.h"
#include "utilities.h"

// database.c -- database, table, record interface/functionality

unsigned int CHUNK = 65535; // Size of CHUNK per buffer

// Create a table record
table_record* record_construct(int size)
{
   int i;
   table_record* record = (table_record*) safe_malloc(sizeof(table_record));
   record->data = (char**) safe_malloc(size*sizeof(char*));
   for (i = 0; i < size; i++) // Initialize all data to null terminator
   {
      *(record->data+i) = NULL;
      safe_strcpy(record->data+i, "\0");
   }
   record->prev = NULL;
   record->next = NULL;
   return record;
}

// Free a record
void record_free(table_record* record, int size)
{
   int i;
   for (i = 0; i < size; i++)
      free(*(record->data+i));
   free(record->data);
   free(record);
}

// Create new table with name, fields, and size (error if already exists)
void table_construct(database* db, char* name, char** fields, int size)
{
   int i;
   table* t = NULL;
   if (table_get(db, name) != NULL) // Table already exists, exit silently
      return;

   t = (table*) safe_malloc(sizeof(table));
   t->name = NULL;
   safe_strcpy(&t->name, name);
   t->fields = (char**) safe_malloc(size*sizeof(char*));
   for (i = 0; i < size; i++) // Initialize fields to NULL, then assign each
   {
      *(t->fields+i) = NULL;
      safe_strcpy(t->fields+i, *(fields+i));
   }
   t->size = size;
   t->prev = NULL;
   t->next = NULL;
   t->first = NULL;
   t->last = NULL;

   // Update the db linked list correctly
   if (db->first == NULL)
      db->first = t;
   
   if (db->last != NULL)
   {
      (db->last)->next = t;
      t->prev = db->last;
   }

   db->last = t;
}

// Free a table, without modifying file
void table_free(table* t)
{
   int i;
   table_record* curr = t->first;
   table_record* next = NULL;
   for (i = 0; i < t->size; i++) // Free each individual field
      free(*(t->fields+i));

   free(t->fields);

   while (curr != NULL) // Free every record in the linked list
   {
      next = curr->next;
      record_free(curr, t->size);
      curr = next;
   }
   free(t->name);
   free(t);
}

// Open or create a database
database* database_open(char* floc)
{
   /* For reading, do the following
    * 1. Check that the filepath is accessible
    * 2. Verify that file is not empty
    * 3. Loop as follows:
    * 4. Read in the size (N)
    * 5. Read in the line, up to \n or EOF
    * 6. Read up to \0 to get the name of the table
    * 7. Read N strings to get fields
    * 8. Continually read strings until \n or EOF
    * 9. For every N strings, add the data as a new record
    * 10. At end of line, check if next character in file is EOF
    * 11. Finalize the table, add to database struct
    * 12. Repeat loop */
   int cap = CHUNK; // max size for line buffer
   char *line = NULL; // line buffer, for line-by-line file read
   int i, c, line_index;
   FILE* temp = NULL;
   database* db = NULL;
   table* prev_t = NULL;
   table* curr_t = NULL;
   table_record* prev = NULL;
   table_record* curr = NULL;
   db = (database*) safe_malloc(sizeof(database));
   db->floc = NULL;
   safe_strcpy(&db->floc, floc);
   db->first = NULL;
   db->last = NULL;
   if (access(floc, F_OK) != -1) // File exists
   {
      temp = fopen(floc, "r"); // Open file for read
      c = fgetc(temp); // Check first char
      if (c == EOF) // Empty file
         return db;
      else
         ungetc(c, temp);

      while (!feof(temp)) // Read to end of file
      {
         curr_t = (table*) safe_malloc(sizeof(table));
         curr_t->size = 0;
         fread(&(curr_t->size), sizeof(int), 1, temp); // Read in an int for size
         line = safe_malloc(cap);
         fgets(line, CHUNK, temp); // Read up to \n or CHUNK-1 characters, last char is \0 (http://www.cplusplus.com/reference/cstdio/fgets/)
         while (strlen(line)+1 == cap && *(line+strlen(line)-1) != '\n') // If size of line + 1 == capacity and last (real) character is not \n
         {
            // Realloc the line
            cap += CHUNK;
            line = safe_realloc(line, cap);
            fgets(line+strlen(line), CHUNK, temp); // Read another chunk
         }
         line_index = 0; // Position of line pointer
         curr_t->name = NULL;
         safe_strcpy(&curr_t->name, line);
         line_index += strlen(curr_t->name) + 1;

         curr_t->fields = (char**) safe_malloc(sizeof(char*)*(curr_t->size));
         for (i = 0; i < curr_t->size; i++) // Go through the line, use pointer arithmetic to set fields
         {
            *(curr_t->fields+i) = NULL;
            safe_strcpy(curr_t->fields+i, line+line_index);
            line_index += strlen(*(curr_t->fields+i)) + 1;
         }
         
         // Table pointers
         curr_t->first = NULL;
         curr_t->last = NULL;
         curr_t->prev = prev_t;
         curr_t->next = NULL;

         prev = NULL;
         while (*(line+line_index) != '\n') // Read records, go up to \n
         {
            curr = (table_record*) safe_malloc(sizeof(table_record));
            curr->prev = prev;
            curr->next = NULL;
            curr->data = (char**) safe_malloc(sizeof(char*)*(curr_t->size));
            for (i = 0; i < curr_t->size; i++) // Move the line pointer around to get record data
            {
               *(curr->data+i) = NULL;
               safe_strcpy(curr->data+i, line+line_index);
               line_index += strlen(*(curr->data+i)) + 1;
            }

            // Manage linked list for table
            if (prev != NULL)
               prev->next = curr;

            if (curr_t->first == NULL)
               curr_t->first = curr;

            curr_t->last = curr;
            prev = curr;
         }
         c = fgetc(temp); // Increment file pointer by 1
         if (c != EOF) // We're not done
            ungetc(c, temp);
         
         // Manage linked list for db
         if (prev_t != NULL)
            prev_t->next = curr_t;

         if (db->first == NULL)
            db->first = curr_t;

         db->last = curr_t;
         prev_t = curr_t;

         // Free the line of input
         free(line);
      }
      // Close db file
      fclose(temp);
   }
   // We're done, return it
   return db;
}

// Write db to file, overwrite existing file
void database_write(database* db)
{
   /* Write in following order:
    * 1. Iterate through each table
    * 2. Write the size, name.
    * 3. Write the fields, in order, delimited by \0.
    * 4. Iterate through every record in the table
    * 5. Per record, write the data, similarly to how #3 works. */
   int i, ret;
   FILE *temp = fopen("tmp", "w"); // Open a tmp file for write (overwrite)
   table* curr_t = db->first;
   table_record* curr = NULL;
   if (temp == NULL) // Err message
   {
      fprintf(stderr, "(database_write) WARNING: unable to write to tmp file\n");
      return;
   }
   else
   {
      while (curr_t != NULL) // Iterate per table
      {
         curr = curr_t->first;
         fwrite(&(curr_t->size), sizeof(int), 1, temp); // Write size
         fputs(curr_t->name, temp); // Write name
         fputc('\0', temp); // fputs does NOT include \0
         for (i = 0; i < curr_t->size; i++) // Write all fields
         {
            fputs(*(curr_t->fields+i), temp);
            fputc('\0', temp);
         }
         while (curr != NULL) // Iterate through records
         {
            for (i = 0; i < curr_t->size; i++) // Write record data
            {
               fputs(*(curr->data+i), temp);
               fputc('\0', temp);
            }  
            curr = curr->next;
         }
         fputc('\n', temp); // Use \n to delimit tables
         curr_t = curr_t->next;
      }

      fclose(temp); // Close the file
      ret = rename("tmp", db->floc); // Rename to db name
      if (ret != 0) 
      {
         fprintf(stderr, "(database_write) WARNING: unable to save database %s/n", db->floc);
      }
   }
}

// Get table in db that has given name, return NULL on fail
table* table_get(database* db, char* name)
{
   table* curr = db->first;
   while (curr != NULL)
   {
      if (safe_strcmp(curr->name, name) == 0)
         return curr;

      curr = curr->next;
   }
   return NULL;
}

// Free all heap memory in db
void database_free(database* db)
{
   table* next = NULL;
   table* curr = db->first;
   while (curr != NULL) // Free all tables
   {
      next = curr->next;
      table_free(curr);
      curr = next;
   }
   free(db->floc);
   free(db);
}

// Write db and free db
void database_close(database* db)
{
   database_write(db);
   database_free(db);
}

// Print table == NAME
void database_print(database* db, char* name)
{
   table* t = table_get(db, name);
   if (t == NULL) // Unable to locate table, exit silently
      return;
   else
      table_print(t);
}

// Print all records in the table, no conditions
void table_print(table* t)
{
   table_record* curr = t->first;
   print_str_array(t->fields, t->size); // Print fields
   while (curr != NULL) // Iterate through records, print data
   {
      print_str_array(curr->data, t->size);
      curr = curr->next;
   }
}

// Select and print all records in table where field=value
void database_select(database* db, char* name, char* field, char* value)
{
   table* t = table_get(db, name);
   if (t == NULL) // unable to locate table, exit silently
      return;
   else
      table_select(t, field, value);
}

// Print all records that have field=value
void table_select(table* t, char* field, char* value)
{
   int index = get_array_index(t->fields, t->size, field);
   table_record* curr = t->first;
   print_str_array(t->fields, t->size);
   
   if (index < 0) // Invalid field
      return;

   while (curr != NULL) // Iterate through records
   {
      if (safe_strcmp(*(curr->data+index), value) == 0) // Found field=value, print it
         print_str_array(curr->data, t->size);
      curr = curr->next;
   }
}

// Delete all records in table where field=value
void database_delete(database* db, char* name, char* field, char* value)
{
   table* t = table_get(db, name);
   if (t == NULL) // unable to locate table, exit silently
      return;
   else
      table_delete(t, field, value);
}

// Delete all records that have field=value
void table_delete(table* t, char* field, char* value)
{
   int index = get_array_index(t->fields, t->size, field);
   table_record* prev = NULL, * next = NULL;
   table_record* curr = t->first;
   
   if (index < 0) // Invalid field
      return;

   while (curr != NULL) // Iterate through records
   {
      prev = curr->prev;
      next = curr->next;
      if (safe_strcmp(*(curr->data+index), value) == 0) // Found field=value
      {
         // Clean record linked list pointers
         if (t->first == curr)
            t->first = curr->next;
         if (t->last == curr)
            t->last = curr->prev;
         if (prev != NULL)
            prev->next = next;
         if (next != NULL)
            next->prev = prev;
         
         // Free the record
         record_free(curr, t->size);
      }
      curr = next;
   }
}

// update all records in table to update_field=update_value, where check_field=check_value
void database_update(database* db, char* name, char* update_field, char* update_value, char* check_field, char* check_value)
{
   table* t = table_get(db, name);
   if (t == NULL) // unable to locate table, exit silently
      return;
   else
      table_update(t, update_field, update_value, check_field, check_value);
}

// Set update_field=update_value if check_field=check_value
void table_update(table* t, char* update_field, char* update_value, char* check_field, char* check_value)
{
   int update_index = get_array_index(t->fields, t->size, update_field);
   int check_index = get_array_index(t->fields, t->size, check_field);
   table_record* curr = t->first;

   if (update_index < 0 || check_index < 0) // Invalid field
      return;

   while (curr != NULL) // Iterate through records
   {
      if (safe_strcmp(*(curr->data+check_index), check_value) == 0) // Found check_field=check_value
         safe_strcpy(curr->data+update_index, update_value); // Update update_field to update_value

      curr = curr->next;
   }
}

// Do an inner join on table name1 and table name2 where name1.field1=name2.field2
void database_join(database* db, char* name1, char* field1, char* name2, char* field2)
{
   table* t1 = table_get(db, name1);
   table* t2 = table_get(db, name2);
   if (t1 == NULL || t2 == NULL || t1 == t2) // At least one bad table or both are identical
      return;
   else
      table_join(t1, field1, t2, field2);
}

// Print all joined rows where t1.field1.value=t2.field2.value
void table_join(table* t1, char* field1, table* t2, char* field2)
{
   int t1_index = get_array_index(t1->fields, t1->size, field1);
   int t2_index = get_array_index(t2->fields, t2->size, field2);
   table_record* curr_t1 = t1->first;
   table_record* curr_t2 = t2->first;
  
   print_2_str_array(t1->fields, t1->size, t2->fields, t2->size); // All fields

   if (t1_index < 0 || t2_index < 0) // One of the fields specified does not exist
      return;

   // This is A*B runtime (length of t1 * length of t2)
   // Not optimal but not terrible
   while (curr_t1 != NULL) // Iterate through records in t1
   {
      curr_t2 = t2->first;
      while (curr_t2 != NULL) // Iterate through records in t2
      {
         if (safe_strcmp(*(curr_t1->data+t1_index), *(curr_t2->data+t2_index)) == 0) // t1.field1 = t2.field2
            print_2_str_array(curr_t1->data, t1->size, curr_t2->data, t2->size); // Print row
         curr_t2 = curr_t2->next;
      }
      curr_t1 = curr_t1->next;
   }
}

// Insert field=value into table name in db
void database_insert(database* db, char* name, char* field, char* value)
{
   table* t = table_get(db, name);
   if (t == NULL) // unable to locate table, exit silently
      return;
   else
      table_insert(t, field, value);
}

// Add new record to bottom of table
void database_add(database* db, char* name)
{
   table* t = table_get(db, name);
   if (t == NULL) // unable to locate table, exit silently
      return;
   else
      table_add(t);
}

// Add new record to bottom of table, given table struct
void table_add(table* t)
{
   table_record* new = record_construct(t->size);
   if (t->first == NULL) // No existing records
   {
      t->first = new;
   }
   else // Chain to existing records
   {
      (t->last)->next = new;
      new->prev = t->last;
   }
   t->last = new;
}

// Insert field/value to latest record
void table_insert(table* t, char* field, char* value)
{
   int index;
   table_record* new = t->last; 
   
   index = get_array_index(t->fields, t->size, field);
   if (index < 0) // Unable to locate field, return silently
      return;
   else // Set field to new value
      safe_strcpy(new->data+index, value);
}

// Remove table name from db
void database_destroy(database* db, char* name)
{
   table* t = table_get(db, name);
   if (t == NULL) // unable to locate table, return silently
      return;
   else
      table_destroy(db, t);
}

// Adjust linked list table pointers, free table to remove table
void table_destroy(database* db, table* t)
{
   // Fix the linked list pointers
   if (db->first == t)
      db->first = t->next;

   if (db->last == t)
      db->last = t->prev;

   if (t->prev != NULL)
      (t->prev)->next = t->next;

   if (t->next != NULL)
      (t->next)->prev = t->prev;

   table_free(t); // Free table and all records inside it
}
