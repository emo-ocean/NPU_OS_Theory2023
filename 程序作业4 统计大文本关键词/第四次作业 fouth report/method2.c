       #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_PARTS 4  // Number of parts to divide the file

int count_keyword(const char *filename, const char *keyword, int start_line, int end_line) {
    FILE *file;
    char buffer[1000];
    int count = 0;

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    int line_number = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL && line_number < end_line) {
        line_number++;
        if (line_number >= start_line && line_number <= end_line) {
            char *pos = buffer;
            while ((pos = strstr(pos, keyword)) != NULL) {
                count++;
                pos += strlen(keyword);
            }
        }
    }

    fclose(file);

    return count;
    }

int main() {
    char *filename = "textfile.txt";
    char keyword[] = "example";  // Keyword to search

    int lines_per_part;
    int remainder_lines;
    int total_lines = 0;
    clock_t start_time, end_time;
    double cpu_time_used;

    start_time = clock();  // 记录程序开始执行的时间

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Count the total number of lines in the file
    char buffer[1000];
    while (fgets(buffer, sizeof(buffer), file) != NULL)
        total_lines++;

    fclose(file);

    // Calculate lines per part and remainder lines
    lines_per_part = total_lines / MAX_PARTS;
    remainder_lines = total_lines % MAX_PARTS;

    int start_line = 1;
    int end_line = lines_per_part;

     for (int i = 0; i < MAX_PARTS; i++) {
        pid_t pid = fork();

        if (pid == 0) {  // Child process
            if (i == MAX_PARTS - 1)  // Adjust end line for the last part
                end_line += remainder_lines;

            int keyword_count = count_keyword(filename, keyword, start_line, end_line);
            printf("Child process %d found the keyword '%s' %d times.\n", getpid(), keyword, keyword_count);
            exit(keyword_count);
        } else if (pid < 0) {  // Error in forking
            perror("Error forking process");
            return 1;
        }

        start_line = end_line + 1;
        end_line += lines_per_part;
    }

    int total_count = 0;
    int status;

    // Wait for all child processes to complete and collect their status
    for (int i = 0; i < MAX_PARTS; i++) {
        wait(&status);
        total_count += WEXITSTATUS(status);
    }

    printf("The keyword '%s' appeared in the entire file %d times.\n", keyword, total_count);
    end_time = clock();  // 记录程序结束执行的时间

    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("程序运行时间：%f 秒\n", cpu_time_used);
    return 0;
    }                 