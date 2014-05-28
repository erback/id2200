#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "timer.h"
#include "malloc.h"

#define NUM_BLOCKS 5000

int main(int argc, char *argv[]) {
    int i;
    void *blocks[NUM_BLOCKS];
    srand(time(0));

    

    /* Measure allocation time */
    for (i = 0; i < NUM_BLOCKS; ++i) {
        printf("op_nr,time\n");
        usleep(rand() % 1000);
        TIMER_START();
        blocks[i] = malloc(8177);
        TIMER_STOP();
        printf("%d,%.2f\n", i, TIMER_ELAPSED_US());
    }

    /* Free blocks */
    for (i = 0; i < NUM_BLOCKS; ++i) {
        free(blocks[i]);
    }

    return EXIT_SUCCESS;
}