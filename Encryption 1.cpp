// ==========================================
// Encryptor.h
// ==========================================
#pragma once
#include <string>
#include <random>
#include <Eigen/Dense>

// Matrix-based encryption class
class Encryptor {
private:
    Eigen::MatrixXd m_key_matrix;
    Eigen::MatrixXd m_key_matrix_inv;
    int m_block_size;
    bool m_key_initialized;

    Eigen::MatrixXd generate_key_matrix(const std::string& password, int dimension);

public:
    Encryptor();
    Encryptor(const std::string& password, int lucky_num);

    bool initialize_key(const std::string& password, int lucky_num);
    bool encrypt(const std::string& plaintext, std::string& ciphertext);
    bool decrypt(const std::string& ciphertext, std::string& plaintext);

    int get_block_size() const { return m_block_size; }
    bool is_key_initialized() const { return m_key_initialized; }
};

// ==========================================
// Encryptor.cpp
// ==========================================
#include "Encryptor.h"
#include <cmath>

Encryptor::Encryptor() : m_block_size(0), m_key_initialized(false) {}

Encryptor::Encryptor(const std::string& password, int lucky_num) 
    : m_block_size(0), m_key_initialized(false) {
    initialize_key(password, lucky_num);
}

// Generate deterministic matrix from password hash
Eigen::MatrixXd Encryptor::generate_key_matrix(const std::string& password, int dimension) {
    Eigen::MatrixXd mat = Eigen::MatrixXd::Zero(dimension, dimension);
    
    std::hash<std::string> str_hash;
    std::mt19937 rand_num(str_hash(password));
    std::uniform_real_distribution<double> dist(-99.9, 66.6);

    for (int i = 0; i < dimension; i++) {
        for (int k = 0; k < dimension; k++) {
            mat(i, k) = dist(rand_num);
            if (i == k) mat(i, k) += 20.06;  // Ensure invertibility
        }
    }
    return mat;
}

bool Encryptor::initialize_key(const std::string& password, int lucky_num) {
    if (lucky_num <= 0 || lucky_num > 9) return false;
    
    m_block_size = lucky_num;
    m_key_matrix = generate_key_matrix(password, lucky_num);
    m_key_matrix_inv = m_key_matrix.inverse();
    m_key_initialized = true;
    return true;
}

bool Encryptor::encrypt(const std::string& plaintext, std::string& ciphertext) {
    if (!m_key_initialized) return false;

    // Pad input to block size
    int padding = (plaintext.size() % m_block_size != 0) 
                  ? m_block_size - (plaintext.size() % m_block_size) : 0;
    std::string padded = plaintext;
    padded.resize(plaintext.size() + padding, 0);

    int num_blocks = padded.size() / m_block_size;
    ciphertext.resize(num_blocks * m_block_size * sizeof(double));
    double* out_ptr = reinterpret_cast<double*>(&ciphertext[0]);

    // Encrypt each block: vector * matrix
    for (int i = 0; i < num_blocks; i++) {
        Eigen::VectorXd vec(m_block_size);
        for (int j = 0; j < m_block_size; j++)
            vec(j) = (unsigned char)padded[i * m_block_size + j];
        
        Eigen::VectorXd enc = m_key_matrix * vec;
        for (int j = 0; j < m_block_size; j++)
            *out_ptr++ = enc(j);
    }
    return true;
}

bool Encryptor::decrypt(const std::string& ciphertext, std::string& plaintext) {
    if (!m_key_initialized) return false;
    if (ciphertext.size() % sizeof(double) != 0) return false;

    int total_doubles = ciphertext.size() / sizeof(double);
    if (total_doubles % m_block_size != 0) return false;

    int num_blocks = total_doubles / m_block_size;
    plaintext.resize(total_doubles);
    const double* in_ptr = reinterpret_cast<const double*>(&ciphertext[0]);
    int idx = 0;

    // Decrypt each block: vector * inverse_matrix
    for (int i = 0; i < num_blocks; i++) {
        Eigen::VectorXd vec(m_block_size);
        for (int j = 0; j < m_block_size; j++)
            vec(j) = *in_ptr++;

        Eigen::VectorXd dec = m_key_matrix_inv * vec;
        for (int j = 0; j < m_block_size; j++)
            plaintext[idx++] = (unsigned char)std::round(dec(j));
    }
    return true;
}

// ==========================================
// FileHandler.h
// ==========================================
#pragma once
#include <string>

// Static utility class for file operations
class FileHandler {
public:
    static bool read_file(const std::string& filename, std::string& content);
    static bool write_file(const std::string& filename, const std::string& content);
    static std::string get_desktop_path();
    static std::string strip_path(std::string path);
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

bool FileHandler::read_file(const std::string& filename, std::string& content) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) return false;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    content.resize(size);
    file.read(&content[0], size);
    return true;
}

bool FileHandler::write_file(const std::string& filename, const std::string& content) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return false;
    file.write(content.data(), content.size());
    return true;
}

std::string FileHandler::get_desktop_path() {
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

std::string FileHandler::strip_path(std::string path) {
    while (!path.empty() && isspace(path.front())) path.erase(0, 1);
    while (!path.empty() && isspace(path.back())) path.pop_back();
    if (!path.empty() && path.front() == '"') path.erase(0, 1);
    if (!path.empty() && path.back() == '"') path.pop_back();
    return path;
}

// ==========================================
// main.cpp
// ==========================================
#include <iostream>
#include <limits>
#include "Encryptor.h"
#include "FileHandler.h"

int main() {
    std::cout << "1. Encrypt\n2. Decrypt\n>> ";
    int choice;
    std::cin >> choice;
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

    std::cout << "File path: ";
    std::string filename;
    std::getline(std::cin, filename);
    filename = FileHandler::strip_path(filename);

    std::string input;
    if (!FileHandler::read_file(filename, input)) {
        std::cout << "Failed to read file\n";
        return 1;
    }

    int lucky_num;
    std::string password;
    std::cout << "Lucky number (1-9): ";
    std::cin >> lucky_num;
    std::cout << "Password: ";
    std::cin >> password;

    Encryptor enc(password, lucky_num);
    std::string output, output_name;

    if (choice == 1) {
        output_name = filename + ".enc";
        enc.encrypt(input, output);
    } else {
        if (filename.substr(filename.size() - 4) != ".enc") {
            std::cout << "Not an .enc file\n";
            return 1;
        }
        output_name = filename.substr(0, filename.size() - 4);
        enc.decrypt(input, output);
    }

    if (FileHandler::write_file(output_name, output))
        std::cout << "Saved: " << output_name << "\n";
    
    return 0;
}
