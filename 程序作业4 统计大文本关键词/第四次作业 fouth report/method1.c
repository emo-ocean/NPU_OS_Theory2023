#include <stdio.h>
#include <string.h>
#include <time.h>
int main() {
    FILE *file;
    char *filename = "textfile.txt";
    char keyword[] = "example";  // Keyword to search
    char buffer[1000];
    int count = 0;
    clock_t start_time, end_time;
    double cpu_time_used;

    start_time = clock();  // 记录程序开始执行的时间

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        char *pos = buffer;
        while ((pos = strstr(pos, keyword)) != NULL) {
            count++;
            pos += strlen(keyword);
        }
    }

    fclose(file);
    
    printf("The keyword '%s' appeared %d times.\n", keyword, count);
    end_time = clock();  // 记录程序结束执行的时间
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("程序运行时间：%f 秒\n", cpu_time_used);
    return 0;
}

              