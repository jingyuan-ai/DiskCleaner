#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <iomanip>

#include "utils/file_helper.h"

// 查找指定目录中的重复文件
std::vector<std::pair<std::string, std::string> > find_duplicate_files(const std::vector<std::string>& dirs) {
    std::unordered_map<std::string, std::string> file_hashes;
    std::vector<std::pair<std::string, std::string> > duplicates;

    for (const auto& directory : dirs) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string file_path = entry.path().string();
                std::string file_hash = ::youya::calculate_file_hash(file_path);
                if (!file_hash.empty()) {
                    if (file_hashes.find(file_hash) != file_hashes.end()) {
                        duplicates.emplace_back(file_path, file_hashes[file_hash]);
                    } else {
                        file_hashes[file_hash] = file_path;
                    }
                }
            }
        }
    }
    return duplicates;
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
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <directory1> <directory2> ..." << std::endl;
        return 1;
    }

    std::vector<std::string> directories;
    for (int i = 1; i < argc; ++i) {
        directories.push_back(argv[i]);
    }

    try {
        auto duplicates = find_duplicate_files(directories);
        print_and_save_duplicates(duplicates);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
