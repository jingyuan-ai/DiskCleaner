import os
import shutil
import hashlib
import argparse
# 增加日志模块
import logging

def calculate_file_hash(file_path):
    """计算文件的 SHA-256 哈希值"""
    # 打印debug日志
    logging.debug(f"Calculating hash for file: {file_path}")
    sha256 = hashlib.sha256()
    with open(file_path, 'rb') as f:
        for chunk in iter(lambda: f.read(4096), b""):
            sha256.update(chunk)
    return sha256.hexdigest()

def find_duplicate_files(dirs):
    """查找指定目录中的重复文件"""
    file_hashes = {}
    duplicates = []

    for directory in dirs:
        for root, _, files in os.walk(directory):
            for file in files:
                file_path = os.path.join(root, file)
                file_hash = calculate_file_hash(file_path)
                if file_hash in file_hashes:
                    duplicates.append((file_path, file_hashes[file_hash]))
                else:
                    file_hashes[file_hash] = file_path
    return duplicates

# 将duplicates记录写到文件中，并打印到屏幕
def print_and_save_duplicates(duplicates):
    if duplicates:
        print("Found duplicate files:")
        with open('records.txt', 'w') as f:
            for dup in duplicates:
                print(f"Duplicate: {dup[0]} and {dup[1]}")
                f.write(f"{dup[0]},{dup[1]}\n")
    else:
        print("No duplicate files found.")

# 将records.txt文件中的记录读取到字典中
def read_records():
    with open('records.txt', 'r') as f:
        records = f.read().splitlines()
    
    file_hashes = {}
    for record in records:
        file_path, file_hash = record.split(',')
        file_hashes[file_hash] = file_path
    
    return file_hashes

# 从命令行中读取一组目录
def parse_args():
    parser = argparse.ArgumentParser(description="Merge directories and find duplicate files.")
    parser.add_argument('source_dirs', nargs='+', help="List of source directories to merge.")
    args = parser.parse_args()
    return args.source_dirs

def main():
    #初始化日志模块
    logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')

    src_dirs = parse_args()
    duplicates = find_duplicate_files(src_dirs)
    print_and_save_duplicates(duplicates)

if __name__ == "__main__":
    main()