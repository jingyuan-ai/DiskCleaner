#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <cstdio>

#include "utils/file_helper.h"

using ::youya::calculate_file_hash;

class CalculateFileHashTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 系统调用使用shell命令创建下面这个文件 echo "Hello World" > test_file1.txt
        std::string cmd1 = "echo \"Hello World\" > test_file1.txt";
        std::string cmd2 = "echo \"Another file content\" > test_file2.txt";
        std::string cmd3 = "touch empty_file.txt";
        std::system(cmd1.c_str());
        std::system(cmd2.c_str());
        std::system(cmd3.c_str());
    }

    void TearDown() override {
        std::remove("test_file1.txt");
        std::remove("test_file2.txt");
        std::remove("empty_file.txt");
    }
};

TEST_F(CalculateFileHashTest, KnownContent) {
    std::string expected_hash1 = "d2a84f4b8b650937ec8f73cd8be2c74add5a911ba64df27458ed8229da804a26";
    std::string expected_hash2 = "672ec673a6cc5020769f42cf04afa53ebb1aec6fd0ef02cd40d8269fd54282c5";

    EXPECT_EQ(calculate_file_hash("test_file1.txt"), expected_hash1);
    EXPECT_EQ(calculate_file_hash("test_file2.txt"), expected_hash2);
}

TEST_F(CalculateFileHashTest, NonexistentFile) {
    EXPECT_EQ(calculate_file_hash("nonexistent_file.txt"), "");
}

TEST_F(CalculateFileHashTest, EmptyFile) {
    std::string expected_hash_empty = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
    EXPECT_EQ(calculate_file_hash("empty_file.txt"), expected_hash_empty);
}