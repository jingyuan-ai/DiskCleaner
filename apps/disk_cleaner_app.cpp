#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <iomanip>
#include <thread>

#include "utils/file_helper.h"
#include "log/log.h"

// 计算所有文件的哈希值
void hash_all_files(const std::vector<std::string>& dirs) {
    std::vector<std::string> files;
    files.reserve(1000000);

    for (const auto& directory : dirs) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                files.emplace_back(entry.path().string());
            }
        }
    }
    LOGI << "Found " << files.size() << " files";

    const int cpu_count = std::thread::hardware_concurrency();
    LOGI << "Using " << cpu_count << " threads";

    int files_per_thread = files.size() / cpu_count;
    std::vector<std::thread> threads;
    threads.reserve(cpu_count);
    std::vector<std::vector<std::pair<std::string, std::string>>> file_hash_list;
    file_hash_list.reserve(cpu_count);

    for (int i = 0; i < cpu_count; ++i) {
        file_hash_list.emplace_back();

        int start = i * files_per_thread;
        int end = (i == cpu_count - 1) ? files.size() : (i + 1) * files_per_thread;
        file_hash_list[i].reserve(end - start);

        auto& file_hashes = file_hash_list[i];
        threads.emplace_back([start, end, &files, &file_hashes] {
            LOGI << "Thread " << std::this_thread::get_id() << " processing files " << start << " to " << end;
            for (int j = start; j < end; ++j) {
                const auto& file_path = files[j];
                std::string file_hash = ::youya::calculate_file_hash(file_path);
                LOGI << file_path << "," << file_hash;
                file_hashes.emplace_back(file_path, file_hash);
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }

    // 将file_hashes_list中的所有哈希值和文件路径写入文件
    const std::string hash_file_name = "hashes.txt";
    LOGI << "Writing hashes to " << hash_file_name;
    std::ofstream hash_file(hash_file_name);
    for (const auto& file_hashes : file_hash_list) {
        for (const auto& file_hash : file_hashes) {
            hash_file << file_hash.first << "," << file_hash.second << std::endl;
        }
    }
}

// 将重复文件记录写到文件中，并打印到屏幕
void print_and_save_duplicates(const std::vector<std::pair<std::string, std::string>>& duplicates) {
    if (!duplicates.empty()) {
        std::cout << "Found duplicate files:" << std::endl;
        std::ofstream record_file("records.txt");
        for (const auto& dup : duplicates) {
            std::cout << "Duplicate: " << dup.first << " and " << dup.second << std::endl;
            record_file << dup.first << "," << dup.second << std::endl;
        }
    } else {
        std::cout << "No duplicate files found." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <directory1> <directory2> ..." << std::endl;
        return 1;
    }

    std::vector<std::string> directories;
    for (int i = 1; i < argc; ++i) {
        directories.push_back(argv[i]);
    }

    try {
        hash_all_files(directories);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
