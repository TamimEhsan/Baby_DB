#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>


// here we dont need to define the constants again because we have already defined them in table.h
// and cursor is included in table.h but we still declare them as extern for readability
extern const uint32_t ROW_SIZE ;
extern const uint32_t ROWS_PER_PAGE ;

typedef struct{
    Table* table;
    uint32_t row_num;
    bool end_of_table;
} Cursor;

Cursor* create_a_table_start_cursor(Table* table){
    Cursor* cursor = (Cursor*)malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->row_num = 0;
    cursor->end_of_table = (table->num_rows == 0);
    return cursor;
}

Cursor* create_a_table_end_cursor(Table* table){
    Cursor* cursor = (Cursor*)malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->row_num = table->num_rows;
    cursor->end_of_table = true;
    return cursor;
}

void* cursor_value(Cursor* cursor) {
  uint32_t row_num = cursor->row_num;
  uint32_t page_num = row_num / ROWS_PER_PAGE;
  void *page = get_page(cursor->table->pager, page_num);
  uint32_t row_offset = row_num % ROWS_PER_PAGE;
  uint32_t byte_offset = row_offset * ROW_SIZE;
  return page + byte_offset;
}

void cursor_advance(Cursor* cursor){
    cursor->row_num += 1;
    if(cursor->row_num >= cursor->table->num_rows){
        cursor->end_of_table = true;
    }
}