/* syscall.c
 *
 * Group Members Names and NetIDs:
 *   1.
 *   2.
 *
 * ILab Machine Tested on:
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>                                                                                
#include <sys/syscall.h>

double avg_time = 0;
#define INTERVAL 1000

int main(int argc, char *argv[]) {

    /* Implement Code Here */

    // Remember to place your final calculated average time
    // per system call into the avg_time variable
    struct timeval start_timeval;
    struct timeval end_timeval;
    double sum; 

    for (int i = 0; i < INTERVAL; i++) {
        gettimeofday(&start_timeval, NULL);
        int pid = getpid();
        gettimeofday(&end_timeval, NULL);
        sum += end_timeval.tv_usec - start_timeval.tv_usec;
    }
    
    avg_time = sum / INTERVAL;
    printf("Average time per system call is %f microseconds\n", avg_time);

    return 0;
}