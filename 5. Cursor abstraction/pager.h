#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

extern const uint32_t ID_SIZE ;
extern const uint32_t USERNAME_SIZE ;
extern const uint32_t EMAIL_SIZE ;
extern const uint32_t ID_OFFSET ;
extern const uint32_t USERNAME_OFFSET ;
extern const uint32_t EMAIL_OFFSET ;
extern const uint32_t ROW_SIZE ;
extern const uint32_t PAGE_SIZE ;
extern const uint32_t TABLE_MAX_PAGES ;
extern const uint32_t ROWS_PER_PAGE ;
extern const uint32_t TABLE_MAX_ROWS ;

typedef struct {
    int file_descriptor;
    uint32_t file_length;
    void* pages[TABLE_MAX_PAGES];
} Pager;

/**
 * Open a file and return a Pager struct
 * Initialize the file descriptor and file length
 * Initialize the pages array to NULL
*/
Pager* pager_open(const char* filename) {
    int fd = open(filename,
                  O_RDWR |      // Read/Write mode
                      O_CREAT , // Create file if it does not exist
                  S_IWUSR |     // User write permission
                      S_IRUSR   // User read permission
    );
    if (fd == -1) {
        printf("Unable to open file\n");
        exit(EXIT_FAILURE);
    }

    // lseek() moves the file offset of the open file description associated with
    // the file descriptor fd to the argument offset according to the directive
    // returns the resulting offset location as measured in bytes from the
    // beginning of the file.
    off_t file_length = lseek(fd, 0, SEEK_END);

    Pager* pager = (Pager*)malloc(sizeof(Pager));
    pager->file_descriptor = fd;
    pager->file_length = file_length;

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        pager->pages[i] = NULL;
    }

    return pager;
}

/**
 * Get the page by page num
 * If the page is not in the cache, allocate memory and load from file
 * Find the location of the page in the file by page_num * PAGE_SIZE
*/
void* get_page(Pager* pager, uint32_t page_num) {
    if (page_num > TABLE_MAX_PAGES) {
        printf("Tried to fetch page number out of bounds. %d > %d\n",
               page_num, TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    if (pager->pages[page_num] == NULL) {
        // Cache miss. Allocate memory and load from file.
        void* page = malloc(PAGE_SIZE);
        uint32_t num_pages = pager->file_length / PAGE_SIZE;

        // We might save a partial page at the end of the file
        if (pager->file_length % PAGE_SIZE) {
            num_pages += 1;
        }

        if (page_num <= num_pages) {
            lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
            ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
            if (bytes_read == -1) {
                printf("Error reading file: %d\n", errno);
                exit(EXIT_FAILURE);
            }
        }
        pager->pages[page_num] = page;
    }

    return pager->pages[page_num];
}

/**
 * Flush the page to the file
 * Find the location of the page in the file by page_num * PAGE_SIZE
 * Write the page to the file
*/
void pager_flush(Pager* pager, uint32_t page_num, uint32_t size) {
    if (pager->pages[page_num] == NULL) {
        printf("Tried to flush null page\n");
        exit(EXIT_FAILURE);
    }

    off_t offset = lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
    if (offset == -1) {
        printf("Error seeking: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_written = write(pager->file_descriptor, pager->pages[page_num], size);
    if (bytes_written == -1) {
        printf("Error writing: %d\n", errno);
        exit(EXIT_FAILURE);
    }
}