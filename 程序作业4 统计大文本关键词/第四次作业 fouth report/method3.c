#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_THREADS 8
#define MAX_TASKS 16
#define MAX_PARTS 4
#define KEYWORD "example"

typedef struct {
    int start_line;
    int end_line;
} Task;

typedef struct {
    Task tasks[MAX_TASKS];
    int front, rear;
    sem_t mutex, empty, full;
} TaskQueue;

void initTaskQueue(TaskQueue *queue) {
    queue->front = 0;
    queue->rear = 0;
    sem_init(&queue->mutex, 0, 1);
    sem_init(&queue->empty, 0, MAX_TASKS);
    sem_init(&queue->full, 0, 0);
}

void enqueue(TaskQueue *queue, Task task) {
    sem_wait(&queue->empty);
    sem_wait(&queue->mutex);
    queue->tasks[queue->rear] = task;
    queue->rear = (queue->rear + 1) % MAX_TASKS;
    sem_post(&queue->mutex);
    sem_post(&queue->full);
}

Task dequeue(TaskQueue *queue) {
    Task task;
    sem_wait(&queue->full);
    sem_wait(&queue->mutex);
    task = queue->tasks[queue->front];
    queue->front = (queue->front + 1) % MAX_TASKS;
    sem_post(&queue->mutex);
    sem_post(&queue->empty);
    return task;
}

TaskQueue taskQueue;
pthread_t workers[MAX_THREADS];

int count_keyword(const char *filename, int start_line, int end_line) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    char buffer[1000];
    int count = 0;
    int line_number = 0;

    while (fgets(buffer, sizeof(buffer), file) != NULL && line_number < end_line) {
        line_number++;
        if (line_number >= start_line && line_number <= end_line) {
            char *pos = buffer;
            while ((pos = strstr(pos, KEYWORD)) != NULL) {
                count++;
                pos += strlen(KEYWORD);
            }
        }
    }
    
    fclose(file);
    return count;
}

void *worker_thread(void *arg) {
    while (1) {
        Task task = dequeue(&taskQueue);
        if (task.start_line == -1 && task.end_line == -1) {
            break;  // Terminate the thread
        }
        int keyword_count = count_keyword("textfile.txt", task.start_line, task.end_line);
        printf("Thread %ld found the keyword '%s' %d times.\n", pthread_self(), KEYWORD, keyword_count);
    }
    return NULL;
}

int main() {

    initTaskQueue(&taskQueue);

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_create(&workers[i], NULL, worker_thread, NULL);
    }

    //计时开始
    clock_t start_time, end_time;
    double cpu_time_used;
    start_time = clock();  // 记录程序开始执行的时间


    // Divide the file into parts and enqueue tasks
    int total_lines = 0;
    FILE *file = fopen("textfile.txt", "r");
    char buffer[1000];
    while (fgets(buffer, sizeof(buffer), file) != NULL)
        total_lines++;
    fclose(file);

    int lines_per_part = total_lines / MAX_PARTS;
    int remainder_lines = total_lines % MAX_PARTS;

    int start_line = 1;
    int end_line = lines_per_part;
    for (int i = 0; i < MAX_PARTS; i++) {
        if (i == MAX_PARTS - 1)
            end_line += remainder_lines;
        Task task = {start_line, end_line};
        enqueue(&taskQueue, task);
        start_line = end_line + 1;
        end_line += lines_per_part;
    }

    // Terminate worker threads after finishing tasks
    for (int i = 0; i < MAX_THREADS; i++) {
        Task task = {-1, -1};  // Termination task
        enqueue(&taskQueue, task);
    }

    // Wait for worker threads to complete
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(workers[i], NULL);
    }
    end_time = clock();  // 记录程序结束执行的时间

    //计时结束
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("程序运行时间：%f 秒\n", cpu_time_used);
    return 0;

}