#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/* Part 1 - Step 2 to 4: Do your tricks here
 * Your goal must be to change the stack frame of caller (main function)
 * such that you get to the line after "r2 = *( (int *) 0 )"
 */
void segment_fault_handler(int signum) {

    printf("I am slain!\n");

    /* Implement Code Here */
    int *pointer = &signum;
    printf("Address that the pointer contains: %p\n", pointer);
    printf("Address of signum: %p\n", &signum);
    printf("Address of the pointer that points to the int signum: %p\n", &pointer);

    for (int i = 0; i <= 15; i += 1){
        printf("At pointer %p", pointer + i );
        printf("    with i pointer: %d", i);
        printf("    with value %p\n", *(pointer + i));
    }
    pointer += 15;
    // printf("    with value %p\n", pointer);
    *pointer = *pointer + 0x5;

}

int main(int argc, char *argv[]) {

    int r2 = 0;

    /* Part 1 - Step 1: Registering signal handler */
    /* Implement Code Here */
    signal(SIGSEGV, segment_fault_handler);
    r2 = *( (int *) 0 ); // This will generate segmentation fault

    printf("I live again!\n");

    return ;
}