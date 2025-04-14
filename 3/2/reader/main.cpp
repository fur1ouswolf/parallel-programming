#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
#include <chrono>

int main() {
    std::ifstream inFile;

    try {

        inFile.open("shared_file.txt");
        if (!inFile.is_open()) {
            throw std::runtime_error("Can't open file for reading");
        }

        std::cout << "File opened for reading\n";

        std::string line;
        while (std::getline(inFile, line)) {
            std::cout << line << "\n";
        }

        inFile.close();
        std::cout << "File closed\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}