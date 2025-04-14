#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
#include <chrono>

int main() {
    std::ofstream outFile;

    try {
        outFile.open("shared_file.txt");
        if (!outFile.is_open()) {
            throw std::runtime_error("Can't open file for writing");
        }

        std::cout << "File opened for writing\n";

        std::string text;
        std::cout << "Enter text to write to the file (type 'exit' to finish):\n";

        while (true) {
            std::getline(std::cin, text);
            if (text == "exit") break;
            outFile << text << "\n";
        }

        outFile.close();
        std::cout << "File closed\n";

    } catch (const std::exception& e) {
        std::cerr << "Erorr: " << e.what() << "\n";
    }

    std::cout << "Mutex unlocked\n";

    return 0;
}