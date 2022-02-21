#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

int allocate_map(void);
void deallocate_map(void);
int allocate_PID(void);
void free_PID(int pid);

void *doWork(void *args);

#define MIN_PID 300 //inclusive
#define MAX_PID 350 //exclusive

#define NUM_THREADS 100
#define ITERATIONS 3 //each thread has to acquire pid this many times
#define SLEEP_TIME 3 //max time in seconds the thread should sleep to represent work

unsigned long fail = 0;
int *pidArray; //0 means the pid is in use, 1 means it is free
pthread_mutex_t mutex;

int main()
{
    srand((unsigned)time(0));
    pthread_mutex_init(&mutex, NULL);
    if (allocate_map() == -1)
    {
        printf("Failed to allocate PID array.\nExiting Program.\n");
        exit(EXIT_FAILURE);
    }

    pthread_t ptid[NUM_THREADS]; //Array to keep track of thread ids
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&ptid[i], NULL, doWork, NULL);
    }

    //Loop through each thread id, making sure each thread is done
    //so as to not deallocate the pid array while the threads are still working
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(ptid[i], NULL);
    }

    deallocate_map();
    printf("%d times process can not obtain a PID\n", fail);
    printf("***DONE***\n");
    return 0;
}

//Creates and initializes data structure for representing pids
//returns -1 if unsuccessful, 1 if successful
int allocate_map(void)
{
    int allocSuccess = 1;
    //Making the array be from 0 to MAX_PID to speed up
    //execution (less calculations) at the cost of some space
    pidArray = (int *)calloc(MAX_PID, sizeof(int));
    if (!pidArray) //if pidArray is null, calloc failed
        allocSuccess = -1;
    return allocSuccess;
}

void deallocate_map(void)
{
    free(pidArray);
}

//Allocates and returns the smallest PID available
//returns -1 if unable to allocate a PID
int allocate_PID(void)
{
    int openPID = -1;
    int i;
    //lock before looking through the array for a free PID
    pthread_mutex_lock(&mutex);
    for (i = MIN_PID; openPID == -1 && i < MAX_PID; i++)
        if (pidArray[i] == 0)
        {
            openPID = 1;
            pidArray[i] = 1;
        }
    if (openPID == 1)
        openPID = i - 1;
    else
        fail++;
    //unlock after finished reading/writing to global vars
    //(pidArray and fail)
    pthread_mutex_unlock(&mutex);
    return openPID;
}

void release_PID(int pid)
{
    //if an already free pid is freed, there's no issue,
    //so no need to check if the pid is in use
    //Lock before writing to pidArray
    pthread_mutex_lock(&mutex);
    pidArray[pid] = 0;
    //Unlock after writing to pidArray
    pthread_mutex_unlock(&mutex);
}

void *doWork(void *args)
{
    int i = 0;
    int currentPID;
    while (i < ITERATIONS)
    {
        currentPID = allocate_PID();
        if (currentPID != -1)
        { //successfully acquired a pid
            printf("Thread %d acquired PID %d\n", pthread_self(), currentPID);
            sleep((rand() % SLEEP_TIME) + 1); //waits 1 to SLEEP_TIME seconds
            //if this print statement is after release_PID(), another thread
            //can acquire the thread between releasing and outputting
            printf("Thread %d released PID %d\n", pthread_self(), currentPID);
            release_PID(currentPID);
            i++;
        }
    }
}