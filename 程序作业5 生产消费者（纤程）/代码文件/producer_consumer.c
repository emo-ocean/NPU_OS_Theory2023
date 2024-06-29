#include <stdio.h>
#include <windows.h>

#define BUFFER_SIZE 10
#define NUM_PRODUCERS 5
#define NUM_CONSUMERS 3

CRITICAL_SECTION bufferLock;
HANDLE producerSem, consumerSem;
int buffer[BUFFER_SIZE];
int itemCount = 0;

DWORD WINAPI Producer(void* arg) {
    for (int i = 0; i < 100; i++) {
        // Produce an item
        int item = rand() % 100;

        // Wait if the buffer is full with a timeout
        if (WaitForSingleObject(producerSem, 1000) == WAIT_TIMEOUT) {
            printf("Producer %d timed out waiting for space in the buffer.\n", (int)arg);
            break;  // Exit the loop if timeout occurs
        }

        EnterCriticalSection(&bufferLock);

        // Add item to the buffer
        buffer[itemCount] = item;
        itemCount++;
        printf("Producer %d produced: %d\n", (int)arg, item);

        LeaveCriticalSection(&bufferLock);
        ReleaseSemaphore(consumerSem, 1, NULL);
    }

    return 0;
}

DWORD WINAPI Consumer(void* arg) {
    for (int i = 0; i < 100; i++) {
        // Wait if the buffer is empty with a timeout
        if (WaitForSingleObject(consumerSem, 1000) == WAIT_TIMEOUT) {
            printf("Consumer %d timed out waiting for items in the buffer.\n", (int)arg);
            break;  // Exit the loop if timeout occurs
        }

        EnterCriticalSection(&bufferLock);

        // Consume an item
        int item = buffer[itemCount - 1];
        itemCount--;
        printf("Consumer %d consumed: %d\n", (int)arg, item);

        LeaveCriticalSection(&bufferLock);
        ReleaseSemaphore(producerSem, 1, NULL);
    }

    return 0;
}


int main() {
    InitializeCriticalSection(&bufferLock);
    producerSem = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, NULL);
    consumerSem = CreateSemaphore(NULL, 0, BUFFER_SIZE, NULL);
    HANDLE producerThreads[NUM_PRODUCERS];
    HANDLE consumerThreads[NUM_CONSUMERS];

    DWORD startTime = GetTickCount();

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        producerThreads[i] = CreateThread(NULL, 0, Producer, (LPVOID)(i + 1), 0, NULL);
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        consumerThreads[i] = CreateThread(NULL, 0, Consumer, (LPVOID)(i + 1), 0, NULL);
    }

    // Wait for all producer threads to finish
    WaitForMultipleObjects(NUM_PRODUCERS, producerThreads, TRUE, INFINITE);

    // Wait for all consumer threads to finish
    WaitForMultipleObjects(NUM_CONSUMERS, consumerThreads, TRUE, INFINITE);

    DWORD endTime = GetTickCount();
    printf("running time is:%ums\n",endTime - startTime);

    for (int i = 0; i < NUM_PRODUCERS; i++) CloseHandle(producerThreads[i]);
    for (int i = 0; i < NUM_CONSUMERS; i++) CloseHandle(consumerThreads[i]);
    CloseHandle(producerSem);
    CloseHandle(consumerSem);
    DeleteCriticalSection(&bufferLock);

    return 0;
}
