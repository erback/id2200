#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include "malloc.h"

#define NUM_ALLOCS 1000
#define NUM_BLOCK_SIZES 11

int main(int argc, char *argv[]) {
    int i;
   
    printf("blocksize,avgtime\n");
    for (i = 0; i < NUM_BLOCK_SIZES; ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return EXIT_FAILURE;
        } else if (pid == 0) {
            int j;
            double avg_time = 0;
            void *blocks[NUM_ALLOCS];
            size_t block_sizes[NUM_BLOCK_SIZES] = {
                64, 128, 256, 512, 1024, 2048,
                4096, 8192, 16384, 32768, 65536
            };

            malloc(42); /* Get things into cache */

            struct timeval start, end; double elapsed;


            /* Measure allocations */
            for (j = 0; j < NUM_ALLOCS; ++j) {
                gettimeofday(&start, NULL);
                blocks[j] = malloc(block_sizes[i]);
                gettimeofday(&end, NULL);
                elapsed = (end.tv_sec - start.tv_sec) * 1000000;
                elapsed = (elapsed + (end.tv_usec - start.tv_usec));
                avg_time += elapsed;
            }
            avg_time /= NUM_ALLOCS;
            printf("%lu,%.2f\n", block_sizes[i], avg_time);

            /* Free allocations */
            for (j = 0; j < NUM_ALLOCS; ++j) {
                free(blocks[j]);
            }

            exit(EXIT_SUCCESS);
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
    printf("%d",STRATEGY);
    return EXIT_SUCCESS;
}