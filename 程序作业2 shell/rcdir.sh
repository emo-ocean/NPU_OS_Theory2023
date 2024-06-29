#!/bin/bash

# 检查错误
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 -t <extension> <directory>"
    exit 1
fi

# 检查参数 -t
if [ "$1" != "-t" ]; then
    echo "Invalid option. Use -t to specify the file extension."
    exit 1
fi

# 命名
extension="$2"
directory="$3"

# 检查目录是否存在
if [ ! -d "$directory" ]; then
    echo "Error: $directory is not a directory."
    exit 1
fi

# 递归函数统计文件数量
count_files() {
    local path="$1"
    local extension="$2"
    local count=0

    # 递归遍历目录
    for file in "$path"/*; do
        if [ -d "$file" ]; then
            count=$((count + $(count_files "$file" "$extension")))
        elif [ "${file##*.}" == "$extension" ]; then
            count=$((count + 1))
        fi
    done

    echo "$count"
}

# 调用函数
count=$(count_files "$directory" "$extension")
echo "Number of .$extension files in $directory: $count"