#include <windows.h>
#include <stdio.h>

#define NUM_FIBERS 8
#define NUM_LOTTERY_TICKETS 1000

LPVOID fibers[NUM_FIBERS];
LPVOID currentFiber;

// Custom Fiber structure to hold lottery tickets
typedef struct {
    LPVOID fiber;
    int tickets;
} FiberInfo;

FiberInfo fiberInfo[NUM_FIBERS];

//纤程执行函数
VOID CALLBACK FiberFunc(PVOID lpFiberParameter) {
    while (1) {
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
        fiberInfo[i].tickets = NUM_LOTTERY_TICKETS / NUM_FIBERS;
    }

    for(int i=0;i<1000;i++) {
        // 彩票延迟
        int winningTicket = rand() % NUM_LOTTERY_TICKETS;
        int winnerIndex = 0;

        for (int i = 0; i < NUM_FIBERS; i++) {
            winningTicket -= fiberInfo[i].tickets;
            if (winningTicket < 0) {
                winnerIndex = i;
                break;
            }
        }

        SwitchToFiber(fiberInfo[winnerIndex].fiber);
    }

    // Cleanup
    for (int i = 0; i < NUM_FIBERS; i++) {
        DeleteFiber(fibers[i]);
    }

    DWORD endTime = GetTickCount();
    printf("running time is:%ums\n",endTime - startTime);

    return 0;
}
