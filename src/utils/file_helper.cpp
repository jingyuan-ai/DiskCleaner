#include "file_helper.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <openssl/evp.h>
#include <sstream>

namespace youya {

// 计算文件的 SHA-256 哈希值
std::string calculate_file_hash(const std::string& file_path) {
    try {
        std::ifstream file(file_path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Could not open file: " + file_path);
        }

        EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
        if (mdctx == nullptr) {
            throw std::runtime_error("Could not create EVP_MD_CTX");
        }

        if (EVP_DigestInit_ex(mdctx, EVP_sha256(), nullptr) != 1) {
            EVP_MD_CTX_free(mdctx);
            throw std::runtime_error("Could not initialize digest context");
        }

        char buffer[4096];
        while (file.read(buffer, sizeof(buffer))) {
            if (EVP_DigestUpdate(mdctx, buffer, file.gcount()) != 1) {
                EVP_MD_CTX_free(mdctx);
                throw std::runtime_error("Could not update digest");
            }
        }
        if (EVP_DigestUpdate(mdctx, buffer, file.gcount()) != 1) {
            EVP_MD_CTX_free(mdctx);
            throw std::runtime_error("Could not update digest");
        }

        unsigned char hash[EVP_MAX_MD_SIZE];
        unsigned int hash_len;
        if (EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1) {
            EVP_MD_CTX_free(mdctx);
            throw std::runtime_error("Could not finalize digest");
        }

        EVP_MD_CTX_free(mdctx);

        std::ostringstream oss;
        for (unsigned int i = 0; i < hash_len; ++i) {
            oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return oss.str();
    } catch (const std::exception& e) {
        std::cerr << "Error calculating hash for file " << file_path << ": " << e.what() << std::endl;
        return "";
    }
}

}