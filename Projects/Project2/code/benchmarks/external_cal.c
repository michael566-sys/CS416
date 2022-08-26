#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include <pthread.h>
#include "../rpthread.h"

#define DEFAULT_THREAD_NUM 2
#define RAM_SIZE 160
#define RECORD_NUM 10
#define RECORD_SIZE 4194304 // 2^22 power

/* Global variables */
pthread_mutex_t   mutex;
int thread_num;
int* counter;
pthread_t *thread;
int *mem = NULL;
int sum = 0;
int itr = RECORD_SIZE / 16;

void external_calculate(void* arg) {
	
	int i = 0, j = 0, k = 0;
	int n = *((int*) arg);
	// starting with the input counter n, add thread_num to it to execute only
	// file number that is equal to k + thread_num for each round
	// for each thread, there are at most RECORD_NUM / thread_num numebr of files
	// being processed. 
	//		for example: 
	//		if RECORD_NUM = 10, thread_num = 4
	// 		there are 4 threads that are processing the following file numbers
	//		as shown below: 
	//		thread 1 => 0, 4, 8
	//		thread 2 => 1, 5, 9
	//		thread 3 => 2, 6
	//		thread 4 => 3, 7
	for (k = n; k < RECORD_NUM; k += thread_num) {

		char a[3];
		char path[20] = "./record/";

		sprintf(a, "%d", k); // assign value of k to a string a
		strcat(path, a); // concatenate a after string path
		
		// get the path that for the file, and open it
		FILE *f;
		f = fopen(path, "r");
		if (!f) {
			printf("failed to open file %s, please run ./genRecord.sh first\n", path);
			exit(0);
		}

		// at the f that contains the file
		/*
		each file has 4096 entries of numbers 
		RAM_SIZE 160 bytes
		RECORD_SIZE 4194304 // 2^22 power
		int itr = RECORD_SIZE / 16 = 2^22 / 2^4 = 2^18
		*/
		printf("at file: %d	\n", k);

		for (i = 0; i < itr; ++i) { 
		printf("	at itr: %d	\n", i);
		// how many iterations should it go through to scan the single file
			// every time reads 16 bytes
			// read 16B from nth or kth record into memory from mem[n*4]

			// for each j = 0 to 3, only 4*4 = 16 bytes are reused
			// these 4 bytes are continuously renewed until all the numbers 
			// in one single kth file are finished scanning. 

			// why there are only 160 bytes for RAM? 
			// answer: for each file, there are 4 * 4 bytes of integer size
			// to rotate to store 4 integers at a time. Since there are 10 files.
			// so 16 * 10 = 10 bytes total to store 10 files in order to prevent
			// confusion between threads.
			for (j = 0; j < 4; ++j) {
				// put the next integer into the &mem[k*4 + j]
				fscanf(f, "%d", &mem[k*4 + j]);
				pthread_mutex_lock(&mutex);
				sum += mem[k*4 + j];
				printf("		number: %d	\n", mem[k*4 + j]);
				printf("		current sum: %d	\n", sum);
				pthread_mutex_unlock(&mutex);
			}
			// why the print output always print out different numbers from different
			// files but not continously from one files only? 
			//		answer: this is because all the threads are running at the same 
			//		time one file prints out 4 numbers, then the mutex is acquired 
			//		by another file, so the next 4 numbers printed will be from 
			//		another file
		}
		fclose(f);
	}

	pthread_exit(NULL);
}


void verify() {
	
	int i = 0, j = 0, k = 0;
	char a[3];
	char path[20] = "./record/";

	sum = 0;
	memset(mem, 0, RAM_SIZE);

	for (k = 0; k < 10; ++k) {
		strcpy(path, "./record/");
		sprintf(a, "%d", k);
		strcat(path, a);

		FILE *f;
		f = fopen(path, "r");
		if (!f) {
			printf("failed to open file %s, please run ./genRecord.sh first\n", path);
			exit(0);
		}

		for (i = 0; i < itr; ++i) {
			// read 16B from nth record into memory from mem[n*4]
			for (j = 0; j < 4; ++j) {
				fscanf(f, "%d\n", &mem[k*4 + j]);
				sum += mem[k*4 + j];
			}
		}
		fclose(f);
	}
	printf("verified sum is: %d\n", sum);
}


void sig_handler(int signum) {
	printf("%d\n", signum);
}


int main(int argc, char **argv) {
	
	int i = 0;

	if (argc == 1) {
		thread_num = DEFAULT_THREAD_NUM;
	} else {
		if (argv[1] < 1) {
			printf("enter a valid thread number\n");
			return 0;
		} else
			thread_num = atoi(argv[1]);
	}

	// initialize counter
	counter = (int*)malloc(thread_num*sizeof(int));
	for (i = 0; i < thread_num; ++i)
		counter[i] = i;

	// initialize pthread_t
	thread = (pthread_t*)malloc(thread_num*sizeof(pthread_t));
	mem = (int*)malloc(RAM_SIZE);
	memset(mem, 0, RAM_SIZE);

	pthread_mutex_init(&mutex, NULL);

	struct timespec start, end;
        clock_gettime(CLOCK_REALTIME, &start);
 
	for (i = 0; i < thread_num; ++i)
		pthread_create(&thread[i], NULL, &external_calculate, &counter[i]);
	
	signal(SIGABRT, sig_handler);
	signal(SIGSEGV, sig_handler);

	for (i = 0; i < thread_num; ++i)
		pthread_join(thread[i], NULL);

	clock_gettime(CLOCK_REALTIME, &end);
        printf("running time: %lu micro-seconds\n", 
	       (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000);

	printf("sum is: %d\n", sum);
	pthread_mutex_destroy(&mutex);

	// feel free to verify your answer here:
	verify();
	
	free(mem);
	free(thread);
	free(counter);

	return 0;
}
