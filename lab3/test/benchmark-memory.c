#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <stddef.h>

#include "brk.h"
#include "malloc.h"

#define NUM_REPS 10
#define NUM_ALLOCS 1000

#define MIN_SIZE 100
#define MAX_SIZE 10000

int main(int argc, char *argv[]) {
    int i;

    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_SHARED | MAP_ANONYMOUS;

    ptrdiff_t *heap_change = mmap(NULL, sizeof(ptrdiff_t), prot, flags, -1, 0);
    size_t *allocated = mmap(NULL, sizeof(size_t), prot, flags, -1, 0);

    *heap_change = 0;
    *allocated = 0;

    for (i = 0; i < NUM_REPS; ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            int j, diff;
            size_t size[NUM_ALLOCS];
            char *block[NUM_ALLOCS];
            void *heap_before;

            srand(time(NULL) ^ (getpid() << 16));

            /* Generate random block sizes */
            for (j = 0; j < NUM_ALLOCS; ++j) {
                diff = MAX_SIZE - MIN_SIZE;
                size[j] = MIN_SIZE + (diff == 0 ? 0 : (rand() % diff));
            }

            /* Allocate blocks and measure heap change */
            heap_before = endHeap();
            for (j = 0; j < NUM_ALLOCS; ++j) {
                block[j] = malloc(size[j]);
                *allocated += size[j];
            }
            *heap_change += endHeap() - heap_before;

            /* Free allocated blocks */
            for (j = 0; j < NUM_ALLOCS; ++j) {
                free(block[j]);
            }

            exit(EXIT_SUCCESS);
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
    
    /* Print the overall memory usage in percent */
    printf("%.2f\n", 100.0 * *allocated / *heap_change);

    return EXIT_SUCCESS;
}

    

