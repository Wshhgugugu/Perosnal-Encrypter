// Encryption 1.cpp : This file contains the 'main' function. Program execution begins and ends there.

// --- Library Includes ---
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <random>
#include <thread>
#include <chrono>
#include <iomanip>

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

// --- Function Prototypes ---
// (Declaring all functions up top so Main can see them)
bool decrypt_file(std::string& input, std::string& output, std::string& password);
bool encrypt_file(std::string& input, std::string& output, std::string& password);
bool write_file(const std::string data_to_write, const std::size_t size, const std::string filename);
void user_password_input(std::string &password);
bool load_file_contents(const std::string& filename, std::string& input, std::string& output, std::size_t &size);
std::string getDesktopPath();
Eigen::MatrixXd matrix_create(std::string pass, long lucky_num);


// ==========================================
//              Main Function
// ==========================================
int main(int argc, char* argv[])
{
    bool debug_mode = false;
    
    // 1. Desktop Path Test
    if (debug_mode) {
        std::string desktopPath = getDesktopPath();
        std::string filePath = desktopPath + "/debug.txt";

        std::ofstream file(filePath);
        if (file.is_open()) {
            file << "Hello, this file is saved to your Desktop!" << std::endl;
            file.close();
            std::cout << "File saved to: " << filePath << std::endl;
        }
        else {
            std::cerr << "Failed to save file at: " << filePath << std::endl;
        }
    }

    // 2. Main Logic Variables
    std::string input, output, password, filename;
    int number;
    std::size_t size{ 0 };

    // 3. User Interaction
    if (argc >= 2) {
        std::cout << "Please upload your input file path: ";
        std::cin >> filename;

        load_file_contents(filename, input, output, size);
    }
    else if (argc < 2) {
        std::cout << "Please upload your input file path: ";
        std::getline(std::cin, filename);
    }

	// Getrid of leading/trailing whitespace and quotes
    while (!filename.empty() && isspace(filename.front())) {
        filename.erase(0, 1);
    }
    while (!filename.empty() && isspace(filename.back())) {
        filename.pop_back();
    }
    if (!filename.empty() && filename.front() == '"') {
		filename.erase(0, 1);
    }
    if (!filename.empty() && filename.back() == '"') {
		filename.pop_back();
    }

    if (!load_file_contents(filename, input, output, size)) {
        std::cout << "File import failed, exiting...\n";
    }

    long lucky_num;
	std::cout << "Please type in a one digit lucky number:" << std::endl;
    std::cin >> lucky_num;
	Eigen::MatrixXd mat1 = matrix_create(password, lucky_num);

   /* write_file(input, size);
    std::cout << "Test file 'encrypted.txt' created. Please rename it to original extension and check if it works!" << std::endl;*/

    std::cout << "Please select an option:\n";
    std::cout << "1. Encrypt a file\n";
    std::cout << "2. Decrypt a file\n";
    std::cin >> number;

    // Create output file name
    std::string new_name{};
    if (number == 1) {
		new_name = filename + ".enc";
    }
    else if (number == 2) {
        while (filename.size() <= 4 || filename.substr(filename.size() - 4, 4) != ".enc") {
			std::cout << "Not an encrypted file, exiting...\n";
            std::cout << "Please upload your input file path: ";
            std::cin >> filename;
        }
        filename.erase(filename.end() - 4, filename.end());
        new_name = filename;
    }

    std::cout << std::endl << "Output file name: " << new_name << std::endl;

    if (number == 1) {
        user_password_input(password);
        encrypt_file(input, output, password);
    }
    if (number == 2) {
        user_password_input(password);
        decrypt_file(input, output, password);
    }

    if (output.empty()) {
        std::cout << "No input provided, exiting...\n";
        return 0;
    }
    else if (write_file(output, size, new_name)) {
        std::cout << "Saved successfully!" << std::endl << "Filename: " << filename;
    }
    else
		"File save failed, exiting...\n";

    return 0;
}


// ==========================================
//          Function Definitions
// ==========================================
bool decrypt_file(std::string& input, std::string& output, std::string& password) {
    return 0;
}

bool encrypt_file(std::string& input, std::string& output, std::string& password) {
    return 0;
}

// Write file (Version 1: takes simple string)
bool write_file(const std::string data_to_write, const std::size_t total_size, const std::string new_name) {
    std::ofstream outFile(new_name, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "File failed to open...Please try again" << std::endl;
        return 0;
    }
    else {
        std::cout << "Processing file, please wait for a moment..." << std::endl;
        // Logic preserved: m_after_enc is empty here
        outFile.write(&data_to_write[0], total_size);
    }
    return 1;
}

void user_password_input(std::string &password) {
    std::string pass;
    std::cout << "Please type in a password:" << std::endl;
    std::cin >> pass;
    password = pass;
}

// Function that ask the user for file path and verifies the contents
bool load_file_contents(const std::string& filename, std::string& input, std::string& output, std::size_t &size) {
    // Setting the both pointers to the end and the beggining to get the file size
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }
    else {
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
}

// Function to get the desktop path based on the operating system
std::string getDesktopPath() {
#if defined(_WIN32)
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, path))) {
        return std::string(path);
    }
    else {
        return ".";
    }
#elif defined(__linux__) || defined(__APPLE__)
    const char* home = getenv("HOME");
    if (home != nullptr) {
        return std::string(home) + "/Desktop";
    }
    else {
        return ".";
    }
#else
    return ".";
#endif
}

Eigen::MatrixXd matrix_create(std::string pass, long lucky_num) {
    // Logic preserved: row_size is initialized to 0
    long row_size{ lucky_num };
    Eigen::MatrixXd mat1(row_size, row_size);
    std::hash<std::string> str_hash;
	std::size_t seed = str_hash(pass);
    std::mt19937 rand_num(seed);

    // Add identity matrix to ensure it invertible
    for (int i = 0; i < row_size; i++) {
        for (int k = 0; k < row_size; k++) {
            if (i == k) {
                mat1(i, k) += 1;
            }
        }
    }
    return mat1;
}
