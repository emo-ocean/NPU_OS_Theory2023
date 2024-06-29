#include <windows.h>
#include <stdio.h>

#define NUM_FIBERS 8

LPVOID fibers[NUM_FIBERS];
LPVOID currentFiber;
int currentIndex = 0;

// 纤程执行函数
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
    }

    for(int i=0;i<1000;i++) {
        SwitchToFiber(fibers[currentIndex]);
        currentIndex = (currentIndex + 1) % NUM_FIBERS;
    }

    // Cleanup
    for (int i = 0; i < NUM_FIBERS; i++) {
        DeleteFiber(fibers[i]);
    }
    DWORD endTime = GetTickCount();
    printf("running time is:%ums\n",endTime - startTime);
    return 0;
}
