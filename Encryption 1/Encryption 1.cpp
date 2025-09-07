// Encryption 1.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <zlib.h>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <random>
#if defined(_WIN32)
#include <windows.h>
#include <shlobj.h>   // SHGetFolderPath
#elif defined(__linux__) || defined(__APPLE__)
#include <cstdlib>    // getenv
#include <unistd.h>
#endif

// Defining all function prototypes
bool decrypt_file(std::string& input, std::string& output, std::string& password);
bool encrypt_file(std::string& input, std::string& output, std::string& password);
void read_file(std::string& input, std::string& output);
bool write_file(const std::string data_to_write);
void user_password_input();
bool load_file_contents(const std::string& filename, std::string& input1, std::string& output1);
bool write_file(std::string output, int max_line);
std::string getDesktopPath();

int main(int argc, char* argv[])
{
    // To get the path
    std::string desktopPath = getDesktopPath();
    std::string filePath = desktopPath + "/output.txt";

    std::ofstream file(filePath);
    if (file.is_open()) {
        file << "Hello, this file is saved to your Desktop!" << std::endl;
        file.close();
        std::cout << "File saved to: " << filePath << std::endl;
    }
    else {
        std::cerr << "Failed to save file at: " << filePath << std::endl;
    }

	// Main function
    std::string input, output, password, input1, output1, filename;
    int number;
	std::cout << "Please upload your input file path: ";
	load_file_contents(filename, input1, output1);
    if(argc < 2) {
		std::cout << "Please upload the input file path as the second command line argument." << std::endl;
		std::cout << "(The order is: 'The path of this program' 'The path of the input file')" << std::endl;
	}
    if(!load_file_contents(filename, input1, output1)) {
		std::cout << "File import failed, exiting...\n";
    }
    std::cout << "Please select an option:\n";
	std::cout << "1. Encrypt a file\n";
	std::cout << "2. Decrypt a file\n";
    std::cin >> number;
    if (number == 1) {
        encrypt_file(input, output, password);
    }
    if (number == 2) {
        decrypt_file(input, output, password);
    }
	read_file(input1, output1);
    if(output.empty()) {
        std::cout << "No input provided, exiting...\n";
        return 0;
	}
    else {
        std::cout << "File loading...";
    }
	void user_password_input();
    return 0;
}

bool decrypt_file(std::string &input, std::string &output, std::string &password) {
    return 0;
}

bool encrypt_file(std::string& input, std::string& output, std::string& password) {
    return 0;
}

void read_file(std::string& input1, std::string& output1) {
    std::ifstream file(input1);
    if (file.is_open()) {
        std::cout << "Reading from file..." << std::endl;
        std::string line;
        while (std::getline(file, line)) {
            output1 += line + "\n";
        }
        file.close();
    }
    else {
        std::cout << "Could not open the file - '" << input1 << "'" << std::endl;
    }
}

// This function is to hold the message after the encryption
bool write_file(const std::string data_to_write) {
    std::string m_after_enc;
    std::ofstream outFile("blank_file.txt");
    if (!outFile.is_open()) {
        std::cerr << "File failed to open...Please try again" << std::endl;
        return 1;
    }
    else {
        std::cout << "Processing file, please wait for a moment..." << std::endl;
        // max_line stands for the total line number of the file being encrypt

        outFile << m_after_enc;
    }
    return 0;
}

void user_password_input() {
    std::cout << "Please type in the password:" << std::endl;
	std::string password_used;
	std::getline(std::cin, password_used);
}

bool load_file_contents(const std::string& filename, std::string& input1, std::string& output1) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }
    else {
        std::streamsize total_size = file.tellg();
        file.seekg(0, std::ios::beg);

        if (total_size == 0) {
            std::cout << "File is empty." << std::endl;
            return true;
        }
		return true;
    }
}

// This function is to hold the message after the encryption
bool write_file(std::string output, int max_line) {
    std::string m_after_enc;
    std::ofstream outFile("blank_file.txt");
    if (!outFile.is_open()) {
        std::cout << "File failed to open...Please try again" << std::endl;
        return 1;
    }
    else {
        std::cout << "Processing file, please wait for a moment..." << std::endl;
		// To convert string to stream
        std::istringstream input_stream(output);
        // max_line stands for the total line number of the file being encrypt
        for (int i = 0; i < max_line; i++) {
            if (std::getline(input_stream, m_after_enc)) {
                outFile << m_after_enc << std::endl;
            }
        }
    }
    return 0;
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