#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

// 递归统计指定扩展名的文件数量
void countFiles(char *path, char *extension, int *count) {
    struct dirent *entry;
    DIR *dp = opendir(path);  // 打开目录

    if (dp == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    // 遍历目录下的所有内容
    while ((entry = readdir(dp))) {
        if (entry->d_type == DT_DIR) {
            // 跳过当前目录和父目录
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char new_path[1024];
                snprintf(new_path, sizeof(new_path), "%s/%s", path, entry->d_name);
                countFiles(new_path, extension, count);  // 递归进入子目录
            }
        } else {
            char *file_extension = strrchr(entry->d_name, '.');  // 获取文件扩展名
            if (file_extension != NULL && strcmp(file_extension + 1, extension) == 0) {
                (*count)++;
            }
        }
    }

    closedir(dp);  // 关闭目录
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s -t <extension> <directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-t") != 0) {
        printf("Invalid option. Use -t to specify the file extension.\n");
        return EXIT_FAILURE;
    }

    char *extension = argv[2];
    char *directory = argv[3];

    struct stat path_stat;
    stat(directory, &path_stat);  // 获取目录信息

    if (!S_ISDIR(path_stat.st_mode)) {
        printf("Error: %s 不是目录。\n", directory);
        return EXIT_FAILURE;
    }

    int count = 0;
    countFiles(directory, extension, &count);  // 开始统计文件数量

    printf("目录 %s 下扩展名为 .%s 的文件数量为: %d\n", directory, extension, count);

    return EXIT_SUCCESS;
}