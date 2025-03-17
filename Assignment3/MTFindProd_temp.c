//csc139-03
//Assignment3
//Mari Moslehi
//Mac
//Tested on: Mac and ECS

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/timeb.h>
#include <semaphore.h>
#include <stdbool.h>
#include <fcntl.h>           // For O_CREAT, O_EXEC  



#define MAX_SIZE 100000000
#define MAX_THREADS 16
#define RANDOM_SEED 7649
#define MAX_RANDOM_NUMBER 3000
#define NUM_LIMIT 9973

// Global variables
long gRefTime; //For timing
int gData[MAX_SIZE]; //The array that will hold the data

int gThreadCount; //Number of threads
int gDoneThreadCount; //Number of threads that are done at a certain point. Whenever a thread is done, it increments this. Used with the semaphore-based solution
int gThreadProd[MAX_THREADS]; //The modular product for each array division that a single thread is responsible for
volatile bool gThreadDone[MAX_THREADS]; //Is this thread done? Used when the parent is continually checking on child threads

// Semaphores
//sem_t completed; //To notify parent that all threads have completed or one of them found a zero
//sem_t mutex; //Binary semaphore to protect the shared variable gDoneThreadCount
//Semaphore for my mac
sem_t *completed; // To notify parent that all threads have completed or one of them found a zero
sem_t *mutex;     // Binary semaphore to protect the shared variable gDoneThreadCount


int SqFindProd(int size); //Sequential FindProduct (no threads) computes the product of all the elements in the array mod NUM_LIMIT
void *ThFindProd(void *param); //Thread FindProduct but without semaphores
void *ThFindProdWithSemaphore(void *param); //Thread FindProduct with semaphores
int ComputeTotalProduct(); // Multiply the division products to compute the total modular product 
void InitSharedVars();
void GenerateInput(int size, int indexForZero); //Generate the input array
void CalculateIndices(int arraySize, int thrdCnt, int indices[MAX_THREADS][3]); //Calculate the indices to divide the array into T divisions, one division per thread
int GetRand(int min, int max);//Get a random number between min and max

//Timing functions
long GetMilliSecondTime(struct timeb timeBuf);
long GetCurrentTime(void);
void SetTime(void);
long GetTime(void);

int main(int argc, char *argv[]){
    pthread_t tid[MAX_THREADS];
    int indices[MAX_THREADS][3];
    int indexForZero, arraySize, prod;

    // Parsing command-line arguments
    if(argc != 4){
        fprintf(stderr, "Invalid number of arguments!\n");
        exit(-1);
    }
    arraySize = atoi(argv[1]);
    if(arraySize <= 0 || arraySize > MAX_SIZE){
        fprintf(stderr, "Invalid Array Size\n");
        exit(-1);
    }
    gThreadCount = atoi(argv[2]);
    if(gThreadCount > MAX_THREADS || gThreadCount <= 0){
        fprintf(stderr, "Invalid Thread Count\n");
        exit(-1);
    }
    indexForZero = atoi(argv[3]);
    if(indexForZero < -1 || indexForZero >= arraySize){
        fprintf(stderr, "Invalid index for zero!\n");
        exit(-1);
    }

    // Generating input data
    GenerateInput(arraySize, indexForZero);

    // Calculating indices for thread division
    CalculateIndices(arraySize, gThreadCount, indices);

    // Sequential product calculation
    SetTime();
    prod = SqFindProd(arraySize);
    printf("Sequential multiplication completed in %ld ms. Product = %d\n", GetTime(), prod);

    // Threaded computation using simple thread join
    InitSharedVars();
    SetTime();
    for (int i = 0; i < gThreadCount; i++) {
        pthread_create(&tid[i], NULL, ThFindProd, (void *)&indices[i]);
    }
    for (int i = 0; i < gThreadCount; i++) {
        pthread_join(tid[i], NULL);
    }
    prod = ComputeTotalProduct();
    printf("Threaded multiplication with parent waiting for all children completed in %ld ms. Product = %d\n", GetTime(), prod);

    // Reset for next test
    InitSharedVars();
    SetTime();
    // Implement your busy-waiting synchronization scheme here

    // Reset and prepare for semaphore-based synchronization
    InitSharedVars();
    //sem_init(&completed, 0, 0);
    //sem_init(&mutex, 0, 1);
    //New Line
    completed = sem_open("/completed_sem", O_CREAT | O_EXCL, 0644, 0);
    mutex = sem_open("/mutex_sem", O_CREAT | O_EXCL, 0644, 1);

    SetTime();
    for (int i = 0; i < gThreadCount; i++) {
        pthread_create(&tid[i], NULL, ThFindProdWithSemaphore, (void *)&indices[i]);
    }
    for (int i = 0; i < gThreadCount; i++) {
        sem_wait(&completed);  // Wait for semaphore signal for each thread
    }
    prod = ComputeTotalProduct();
    printf("Threaded multiplication with parent waiting on a semaphore completed in %ld ms. Product = %d\n", GetTime(), prod);

    // Clean up semaphores
    //sem_destroy(&completed);
    //sem_destroy(&mutex);
    //New Line
    sem_close(completed);
    sem_unlink("/completed_sem");
    sem_close(mutex);
    sem_unlink("/mutex_sem");

    return 0;
}

// Write a regular sequential function to multiply all the elements in gData mod NUM_LIMIT
// REMEMBER TO MOD BY NUM_LIMIT AFTER EACH MULTIPLICATION TO PREVENT YOUR PRODUCT VARIABLE FROM OVERFLOWING
int SqFindProd(int size) {
    int prod = 1;
    for (int i = 0; i < size; i++) {
        if (gData[i] == 0) return 0;
        prod = (prod * gData[i]) % NUM_LIMIT;
    }
    return prod;
}

// Write a thread function that computes the product of all the elements in one division of the array mod NUM_LIMIT
// REMEMBER TO MOD BY NUM_LIMIT AFTER EACH MULTIPLICATION TO PREVENT YOUR PRODUCT VARIABLE FROM OVERFLOWING
// When it is done, this function should store the product in gThreadProd[threadNum] and set gThreadDone[threadNum] to true
void* ThFindProd(void *param) {
    int threadNum = ((int*)param)[0];
    int startIdx = ((int*)param)[1];
    int endIdx = ((int*)param)[2];
    int prod = 1;

    for (int i = startIdx; i <= endIdx; i++) {
        if (gData[i] == 0) {
            gThreadProd[threadNum] = 0;
            gThreadDone[threadNum] = true;
            return NULL;
        }
        prod = (prod * gData[i]) % NUM_LIMIT;
    }
    gThreadProd[threadNum] = prod;
    gThreadDone[threadNum] = true;
    return NULL;

}

// Write a thread function that computes the product of all the elements in one division of the array mod NUM_LIMIT
// REMEMBER TO MOD BY NUM_LIMIT AFTER EACH MULTIPLICATION TO PREVENT YOUR PRODUCT VARIABLE FROM OVERFLOWING
// When it is done, this function should store the product in gThreadProd[threadNum]
// If the product value in this division is zero, this function should post the "completed" semaphore
// If the product value in this division is not zero, this function should increment gDoneThreadCount and
// post the "completed" semaphore if it is the last thread to be done
// Don't forget to protect access to gDoneThreadCount with the "mutex" semaphore
void* ThFindProdWithSemaphore(void *param) {
    // Extract parameters
    int threadNum = ((int*)param)[0];
    int startIdx = ((int*)param)[1];
    int endIdx = ((int*)param)[2];

    int prod = 1;  // Initialize product for this segment

    // Compute the product of the segment
    for (int i = startIdx; i <= endIdx; i++) {
        if (gData[i] == 0) {
            gThreadProd[threadNum] = 0;
            sem_post(completed);  // Signal completion because of zero
            return NULL;
        }
        prod = (prod * gData[i]) % NUM_LIMIT;
    }

    gThreadProd[threadNum] = prod;

    // Protect access to shared counter
    sem_wait(mutex);
    gDoneThreadCount++;
    bool isLastThread = (gDoneThreadCount == gThreadCount);  // Check if this is the last thread to finish
    sem_post(mutex);

    // If the last thread, signal the completion of all threads
    if (isLastThread) {
        sem_post(completed);
    }

    return NULL;
}

int ComputeTotalProduct() {
    int i, prod = 1;

	for(i=0; i<gThreadCount; i++)
	{
		prod *= gThreadProd[i];
		prod %= NUM_LIMIT;
	}

	return prod;
}

void InitSharedVars() {
	int i;

	for(i=0; i<gThreadCount; i++){
		gThreadDone[i] = false;
		gThreadProd[i] = 1;
	}
	gDoneThreadCount = 0;
}

// Write a function that fills the gData array with random numbers between 1 and MAX_RANDOM_NUMBER
// If indexForZero is valid and non-negative, set the value at that index to zero
void GenerateInput(int size, int indexForZero) {
	srand(RANDOM_SEED); // Initialize the random number generator
    for (int i = 0; i < size; i++) {
        gData[i] = GetRand(1, MAX_RANDOM_NUMBER);
    }
    if (indexForZero >= 0) {
        gData[indexForZero] = 0; // Set the specified index to zero
    }
}

// Write a function that calculates the right indices to divide the array into thrdCnt equal divisions
// For each division i, indices[i][0] should be set to the division number i,
// indices[i][1] should be set to the start index, and indices[i][2] should be set to the end index
void CalculateIndices(int arraySize, int thrdCnt, int indices[MAX_THREADS][3]) {
	int blockSize = arraySize / thrdCnt;
    int startIdx = 0;
    for (int i = 0; i < thrdCnt; i++) {
        indices[i][0] = i;
        indices[i][1] = startIdx;
        indices[i][2] = startIdx + blockSize - 1;
        if (i == thrdCnt - 1) {
            indices[i][2] = arraySize - 1; // Make sure the last thread covers the rest
        }
        startIdx += blockSize;
    }
}

// Get a random number in the range [x, y]
int GetRand(int x, int y) {
    int r = rand();
    r = x + r % (y-x+1);
    return r;
}

long GetMilliSecondTime(struct timeb timeBuf){
	long mliScndTime;
	mliScndTime = timeBuf.time;
	mliScndTime *= 1000;
	mliScndTime += timeBuf.millitm;
	return mliScndTime;
}

long GetCurrentTime(void){
	long crntTime=0;
	struct timeb timeBuf;
	ftime(&timeBuf);
	crntTime = GetMilliSecondTime(timeBuf);
	return crntTime;
}

void SetTime(void){
	gRefTime = GetCurrentTime();
}

long GetTime(void){
	long crntTime = GetCurrentTime();
	return (crntTime - gRefTime);
}

