#ifndef database_H_
#define database_H_

// database.h -- define record, table, database structs and method signatures

typedef struct table_record   // Row in table
{
   char** data;         // Array of values
   struct table_record* prev; // Previous record pointer
   struct table_record* next; // Next record pointer
} table_record;

typedef struct table
{
   char* name;                // Name identifier
   int size;                     // Num fields
   char** fields;                // Array of fields
   struct table_record* first;// First record 
   struct table_record* last; // Last record
   struct table* prev;        // Previous table
   struct table* next;        // Next table
} table;

typedef struct database
{
   char* floc;                // Filepath to save to
   struct table* first;       // First table
   struct table* last;        // Last table
} database;

// Create a record
table_record* record_construct(int size);
// Free a record
void record_free(table_record* record, int size);
// Create new table, floc points to path of new file (error if already exists)
void table_construct(database* db, char* name, char** fields, int size);
// Free a table, without modifying file
void table_free(table* t);
// Open or create a database
database* database_open(char* floc);
// Write db to file, overwrite existing file
void database_write(database* db);
// Get table from db
table* table_get(database* db, char* name);
// Free db
void database_free(database* db);
// Free pointers, write db
void database_close(database* db);
// Print table == NAME
void database_print(database* db, char* name);
// Print all records in the table, no conditions
void table_print(table* t);
// Run select on db
void database_select(database* db, char* name, char* field, char* value);
// Print all records that have field=value
void table_select(table* t, char* field, char* value);
// Delete in table name
void database_delete(database* db, char* name, char* field, char* value);
// Delete all records that have field=value
void table_delete(table* t, char* field, char* value);
// Update table with name
void database_update(database* db, char* name, char* update_field, char* update_value, char* check_field, char* check_value);
// Set update_field=update_value if check_field=check_value
void table_update(table* t, char* update_field, char* update_value, char* check_field, char* check_value);
// Run an inner JOIN
void database_join(database* db, char* name1, char* field1, char* name2, char* field2);
// Print all joined rows where t1.field1.value=t2.field2.value
void table_join(table* t1, char* field1, table* t2, char* field2);
// Insert table into db
void database_insert(database* db, char* name, char* field, char* value);
// Add record to end of table
void database_add(database* db, char* name);
// Add new record
void table_add(table* t);
// Update last record
void table_insert(table* t, char* field, char* value);
// Remove table from db
void database_destroy(database* db, char* name);
// Free all pointers
void table_destroy(database* db, table* t);
#endif
