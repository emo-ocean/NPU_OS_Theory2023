#include <windows.h>
#include <stdio.h>

#define NUM_FIBERS 8
#define NUM_PRIORITIES 3

LPVOID fibers[NUM_FIBERS];
LPVOID currentFiber;

// Custom Fiber structure to hold priorities
typedef struct {
    LPVOID fiber;
    int priority;
} FiberInfo;

FiberInfo fiberInfo[NUM_FIBERS];

// Fiber worker function
VOID CALLBACK FiberFunc(PVOID lpFiberParameter) {
    while (1) {
        // Your custom task here
        printf("Fiber %p is running\n", lpFiberParameter);
        SwitchToFiber(currentFiber);
    }
}

int main() {
    currentFiber = ConvertThreadToFiber(NULL);

    DWORD startTime = GetTickCount();


    for (int i = 0; i < NUM_FIBERS; i++) {
        fibers[i] = CreateFiber(0, FiberFunc, (LPVOID)i);
        fiberInfo[i].fiber = fibers[i];
        fiberInfo[i].priority = i % NUM_PRIORITIES;
    }

    for(int i=0;i<1000;i++) {
        int highestPriority = NUM_PRIORITIES - 1;
        int nextFiber = -1;

        for (int i = 0; i < NUM_FIBERS; i++) {
            if (fiberInfo[i].priority >= highestPriority) {
                highestPriority = fiberInfo[i].priority;
                nextFiber = i;
            }
        }

        if (nextFiber != -1) {
            SwitchToFiber(fiberInfo[nextFiber].fiber);
        }
    }

    // Cleanup
    for (int i = 0; i < NUM_FIBERS; i++) {
        DeleteFiber(fibers[i]);
    }

    DWORD endTime = GetTickCount();
    printf("running time is:%ums\n",endTime - startTime);

    return 0;
}
