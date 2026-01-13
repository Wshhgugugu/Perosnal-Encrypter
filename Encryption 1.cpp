// ==========================================
// Encryptor.h
// ==========================================
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <random>
#include <thread>
#include <chrono>
#include <iomanip>
#include <limits>

// --- Third-party Libraries ---
#include <Eigen/Dense>
#include <zlib.h>

// --- Platform Specific Includes ---
#if defined(_WIN32)
#include <windows.h>
#include <shlobj.h>    // SHGetFolderPath
#elif defined(__linux__) || defined(__APPLE__)
#include <cstdlib>     // getenv
#include <unistd.h>
#endif

class Encryptor {
private:
    Eigen::MatrixXd mat1;
    Eigen::MatrixXd mat1_inv;
    int block_size;
    bool initialized;

    Eigen::MatrixXd matrix_create(const std::string& pass, long lucky_num);

public:
    Encryptor();
    Encryptor(const std::string& pass, long lucky_num);

    bool init(const std::string& pass, long lucky_num);
    bool encrypt_file(const std::string& input, std::string& output);
    bool decrypt_file(const std::string& input, std::string& output);

    bool is_initialized() const { return initialized; }
};

// ==========================================
// Encryptor.cpp
// ==========================================
#include "Encryptor.h"
#include <cmath>

Encryptor::Encryptor() : block_size(0), initialized(false) {}

Encryptor::Encryptor(const std::string& pass, long lucky_num) 
    : block_size(0), initialized(false) {
    init(pass, lucky_num);
}

Eigen::MatrixXd Encryptor::matrix_create(const std::string& pass, long lucky_num) {
    long row_size = lucky_num;
    Eigen::MatrixXd mat = Eigen::MatrixXd::Zero(row_size, row_size);

    std::hash<std::string> str_hash;
    std::size_t seed = str_hash(pass);
    std::mt19937 rand_num(seed);
    std::uniform_real_distribution<double> dist(-99.9, 66.6);

    for (int i = 0; i < row_size; i++) {
        for (int k = 0; k < row_size; k++) {
            mat(i, k) = dist(rand_num);
            if (i == k) mat(i, k) += 20.06;
        }
    }
    return mat;
}

bool Encryptor::init(const std::string& pass, long lucky_num) {
    if (lucky_num <= 0 || lucky_num > 9) return false;
    
    block_size = lucky_num;
    mat1 = matrix_create(pass, lucky_num);
    mat1_inv = mat1.inverse();
    initialized = true;
    return true;
}

bool Encryptor::encrypt_file(const std::string& input, std::string& output) {
    if (!initialized) return false;

    int original_size = input.size();
    int padding = (original_size % block_size != 0) 
                  ? block_size - (original_size % block_size) : 0;

    std::string padded_input = input;
    padded_input.resize(original_size + padding, 0);

    int num_blocks = padded_input.size() / block_size;
    output.resize(num_blocks * block_size * sizeof(double));
    double* out_ptr = reinterpret_cast<double*>(&output[0]);

    for (int i = 0; i < num_blocks; i++) {
        Eigen::VectorXd vec(block_size);
        for (int j = 0; j < block_size; j++)
            vec(j) = static_cast<double>((unsigned char)padded_input[i * block_size + j]);

        Eigen::VectorXd encrypted_vec = mat1 * vec;
        for (int j = 0; j < block_size; j++)
            *out_ptr++ = encrypted_vec(j);
    }
    return true;
}

bool Encryptor::decrypt_file(const std::string& input, std::string& output) {
    if (!initialized) return false;
    if (input.size() % sizeof(double) != 0) return false;

    int total_doubles = input.size() / sizeof(double);
    if (total_doubles % block_size != 0) return false;

    int num_blocks = total_doubles / block_size;
    output.resize(total_doubles);
    const double* in_ptr = reinterpret_cast<const double*>(&input[0]);
    int current_out_idx = 0;

    for (int i = 0; i < num_blocks; i++) {
        Eigen::VectorXd vec(block_size);
        for (int j = 0; j < block_size; j++)
            vec(j) = *in_ptr++;

        Eigen::VectorXd decrypted_vec = mat1_inv * vec;
        for (int j = 0; j < block_size; j++)
            output[current_out_idx++] = static_cast<unsigned char>(std::round(decrypted_vec(j)));
    }
    return true;
}

// ==========================================
// FileHandler.h
// ==========================================
#pragma once
#include <string>

class FileHandler {
public:
    static bool load_file_contents(const std::string& filename, std::string& input, std::size_t& size);
    static bool write_file(const std::string& data_to_write, std::size_t total_size, const std::string& new_name);
    static std::string getDesktopPath();
    static std::string strip_path(std::string filename);
};

// ==========================================
// FileHandler.cpp
// ==========================================
#include "FileHandler.h"
#include <fstream>
#include <iostream>

#if defined(_WIN32)
    #include <Windows.h>
    #include <ShlObj.h>
#else
    #include <cstdlib>
#endif

bool FileHandler::load_file_contents(const std::string& filename, std::string& input, std::size_t& size) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }

    std::streamsize total_size = file.tellg();
    file.seekg(0, std::ios::beg);
    size = total_size;

    if (total_size == 0) {
        std::cout << "File is empty." << std::endl;
        return true;
    }

    input.resize(total_size);
    file.read(&input[0], total_size);
    return true;
}

bool FileHandler::write_file(const std::string& data_to_write, std::size_t total_size, const std::string& new_name) {
    std::ofstream outFile(new_name, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "File failed to open...Please try again" << std::endl;
        return false;
    }
    std::cout << "Processing file, please wait for a moment..." << std::endl;
    outFile.write(&data_to_write[0], total_size);
    return true;
}

std::string FileHandler::getDesktopPath() {
#if defined(_WIN32)
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, path)))
        return std::string(path);
    return ".";
#else
    const char* home = getenv("HOME");
    return home ? std::string(home) + "/Desktop" : ".";
#endif
}

std::string FileHandler::strip_path(std::string filename) {
    while (!filename.empty() && isspace(filename.front())) filename.erase(0, 1);
    while (!filename.empty() && isspace(filename.back())) filename.pop_back();
    if (!filename.empty() && filename.front() == '"') filename.erase(0, 1);
    if (!filename.empty() && filename.back() == '"') filename.pop_back();
    return filename;
}

// ==========================================
// main.cpp
// ==========================================
#include <iostream>
#include <limits>
#include "Encryptor.h"
#include "FileHandler.h"

int main() {
    std::string input, output, password, filename;
    int number;
    std::size_t size{0};

    std::cout << "Please select an option:\n";
    std::cout << "1. Encrypt a file\n";
    std::cout << "2. Decrypt a file\n";
    std::cin >> number;
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

    std::cout << "Please upload your input file: ";
    std::getline(std::cin, filename);
    filename = FileHandler::strip_path(filename);

    if (!FileHandler::load_file_contents(filename, input, size)) {
        std::cout << "File import failed, exiting...\n";
        return 1;
    }

    long lucky_num;
    std::cout << "Please type in a one digit lucky number:" << std::endl;
    std::cin >> lucky_num;

    std::cout << "Please type in a password:" << std::endl;
    std::cin >> password;

    // Create output file name
    std::string new_name;
    if (number == 1) {
        new_name = filename + ".enc";
    } else if (number == 2) {
        if (filename.size() <= 4 || filename.substr(filename.size() - 4) != ".enc") {
            std::cout << "Not an encrypted file, exiting...\n";
            return 1;
        }
        new_name = filename.substr(0, filename.size() - 4);
    }

    std::cout << "Output file name: " << new_name << std::endl;

    // Create encryptor and process
    Encryptor encryptor(password, lucky_num);

    if (number == 1)
        encryptor.encrypt_file(input, output);
    else if (number == 2)
        encryptor.decrypt_file(input, output);

    if (output.empty()) {
        std::cout << "No input provided, exiting...\n";
        return 0;
    }

    if (FileHandler::write_file(output, output.size(), new_name))
        std::cout << "Saved successfully!" << std::endl << "Filename: " << new_name;
    else
        std::cout << "File save failed, exiting...\n";

    return 0;
}
