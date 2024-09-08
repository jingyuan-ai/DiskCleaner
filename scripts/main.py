# 
# hash.txt文件格式如下，每行包括一个文件路径和对应的SHA-256哈希值，用逗号分隔：
# <filename>,<file_hash>
# 

# 读取上述格式的文件，找出其中hash值相同的文件，并输出到文件中
import os
import logging
from collections import defaultdict

# 封装一个删除文件的函数，要确保文件是存在的，而且不是系统目录
# 删除之后创建一个软链接指向和它相同的文件
def remove_duplicated_file(file, another_file):
    # file不是目录
    if os.path.exists(file) and not os.path.islink(file) and not os.path.isdir(file):
        os.remove(file)
        logging.info(f'Remove file: {file}')
        # 同时touch一个文件，软链接到另一个文件
        os.symlink(another_file, file)

# 测试remove_duplicated_file
def test_remove_duplicated_file():
    file = 'test.txt'
    another_file = 'another.txt'
    with open(file, 'w') as f:
        f.write('test')
    with open(another_file, 'w') as f:
        f.write('another')
    remove_duplicated_file(file, another_file)
    # 检查软链接是否正确
    assert os.path.islink(file)
    # 检查是否链接到了another_file
    assert os.path.realpath(file) == os.path.realpath(another_file)

def parse_all_hash_files(files, result_file_path='duplicated_files.txt'):
    with open(result_file_path, 'w') as fw:
        fw.write('path,hash_value,duplicated,reference\n')
    hash_dict = {}
    for file in files:
        parse_hash_file(file, hash_dict, result_file_path)

def parse_hash_file(file, hash_dict, result_file_path):
    # 打开文件，并解析每一行
    with open(result_file_path, 'a') as fw:
        with open(file, 'r') as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue
                # logging.info(f'Processing line: {line}')
                # 捕获异常，以便处理错误行
                try:
                    path, hash_value = line.split(',')
                except Exception as e:
                    logging.error(f'Error line: {line} with error: {e}')
                    continue
                if len(path) == 0:
                    logging.error(f'No path line: {line}')
                    continue
                if hash_value not in hash_dict:
                    hash_dict[hash_value] = path
                    # fw.write(f'{path},{hash_value},0,"NULL"\n')
                else:
                    # 检查文件名是否相同
                    if os.path.basename(path) == os.path.basename(hash_dict[hash_value]):
                        # logging.info(f'Find duplicate file: {path} and {hash_dict[hash_value]}')
                        fw.write(f'{path},{hash_value},1,{hash_dict[hash_value]}\n')

# 测试parse_hash_file
def test_parse_hash_file():
    # 先创建一个hash_file文件
    hash_file = 'hash_file.txt'
    with open(hash_file, 'w') as f:
        f.write('test.txt,123\n')
        f.write('another.txt,123\n')
        f.write('another_dir/test.txt,123\n')
    # 创建一个result_file文件
    result_file = 'result_file.txt'
    with open(result_file, 'w') as f:
        f.write('path,hash_value,duplicated,reference\n')
    hash_dict = {}
    parse_hash_file(hash_file, hash_dict, result_file)
    # 检查result_file文件内容
    with open(result_file, 'r') as f:
        lines = f.readlines()
        assert len(lines) == 2
        assert lines[1] == 'another_dir/test.txt,123,1,test.txt\n'

# 从命令行中获取文件名
def parse_args():
    import argparse
    parser = argparse.ArgumentParser(description='Find duplicate files according to file hash.')
    parser.add_argument('files', type=str, nargs='+', help='The hash file.')
    # 输出文件路径如果不包含就是默认值
    parser.add_argument('-o', '--output', type=str, default='duplicated_files.txt', help='The output file.')
    # 支持运行测试程序的模式
    parser.add_argument('-t', '--test', action='store_true', help='Run test mode.')
    args = parser.parse_args()
    return args

def test():
    test_remove_duplicated_file()
    test_parse_hash_file()

def main():
    #初始化日志模块
    logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')

    args = parse_args()

    if args.test:
        test()
        return

    parse_all_hash_files(args.files, result_file_path=args.output)

if __name__ == "__main__":
    main()