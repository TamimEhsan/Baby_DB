#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



#include "table.h"



typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length; // ssize_t is a signed size_t
} InputBuffer;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

/**
 * Statement is a struct that represents a statement
 * It will contain further information later
*/
typedef struct {
    StatementType type;
    Row row_to_insert; // only used by insert statement
} Statement;

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;



typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
} ExecuteResult;

/**
 * Generate a new input buffer
*/
InputBuffer* new_input_buffer() {
    InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

/**
 * Close the input buffer
 * Free the memory
*/
void close_input_buffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}


/**
 * Meta commands start with a dot
 * This function will handle them seperately
*/
MetaCommandResult do_meta_command(InputBuffer* input_buffer, Table* table) {
    if ( strcmp(input_buffer->buffer, ".exit") == 0 ) {
        close_input_buffer(input_buffer);
        db_close(table);
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

/**
 * Prepare the statement
 * This function will handle the statements
*/
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if ( strncmp(input_buffer->buffer, "insert", 6) == 0 ) {
        statement->type = STATEMENT_INSERT;
        int args_assigned = sscanf(
            input_buffer->buffer, "insert %d %s %s", &(statement->row_to_insert.id),
            statement->row_to_insert.username, statement->row_to_insert.email
        );
        if (args_assigned < 3) {
            return PREPARE_SYNTAX_ERROR;
        }
        return PREPARE_SUCCESS;
    }

    if ( strcmp(input_buffer->buffer, "select") == 0 ) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult execute_insert(Statement* statement, Table* table) {
    if (table->num_rows >= TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }
    Row* row_to_insert = &(statement->row_to_insert);
    serialize_row(row_to_insert, get_row_slot_position(table,table->num_rows));
    table->num_rows += 1;
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement* statement, Table* table) {
    Row row;
    for (uint32_t i = 0; i < table->num_rows; i++) {
        deserialize_row(get_row_slot_position(table,i), &row);
        print_row(&row);
    }
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_statement(Statement* statement, Table* table) {
    switch (statement->type) {
        case (STATEMENT_INSERT):
            return execute_insert(statement,table);
            break;
        case (STATEMENT_SELECT):
            return execute_select(statement,table);
            break;
    }
}

/**
 * Print the prompt
*/
void print_prompt() { printf("db > "); }

/**
 * Read a line of input
*/
void read_input(InputBuffer* input_buffer) {
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    if (bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    // Ignore trailing newline
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
}


int main() {

    char* filename = "db.db";

    Table* table = db_open(filename);
    InputBuffer* input_buffer = new_input_buffer();
    while (true) {

        // Print the prompt and read input
        print_prompt();
        read_input(input_buffer);

        // Handle the meta commands
        if( input_buffer->buffer[0] == '.' ){
            switch (do_meta_command(input_buffer,table)) {
                case (META_COMMAND_SUCCESS):
                    continue;
                case (META_COMMAND_UNRECOGNIZED_COMMAND):
                    printf("Unrecognized command '%s'.\n", input_buffer->buffer);
                    continue;
            }
        } 

        // Check the type of the statement and validate it
        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            case (PREPARE_SUCCESS):
                break;
            case (PREPARE_SYNTAX_ERROR):
                printf("Syntax error. Could not parse statement.\n");
                continue;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
                continue;
        }

        // Execute the statement; insert and select
        switch (execute_statement(&statement,table)){
            case (EXECUTE_SUCCESS):
                printf("Executed.\n");
                break;
            case (EXECUTE_TABLE_FULL):
                printf("Error: Table full.\n");
                break;
        }

       
    }
}