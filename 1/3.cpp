#include <iostream>
#include <vector>
#include <thread>
#include <iomanip>
#include <random>
#include <chrono>
#include <cassert>

void multiplyRow(std::vector<int>& row, int factor) {
    for (int& val : row) {
        val *= factor;
    }
}

void multiplyMatrixParallel(std::vector<std::vector<int>>& matrix, int factor) {
    std::vector<std::thread> threads;
    for (auto& row : matrix) {
        threads.emplace_back(multiplyRow, std::ref(row), factor);
    }
    for (auto& t : threads) {
        t.join();
    }
}

int main3() {
    int n = 3, m = 4;
    std::vector<std::vector<int>> matrix = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12}
    };

    std::cout << "Original:" << std::endl;
    for (auto& row : matrix) {
        for (auto& val : row) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    int factor;
    std::cout << "Enter factor: ";
    std::cin >> factor;

    multiplyMatrixParallel(matrix, factor);

    std::cout << "Result:\n";
    for (const auto& row : matrix) {
        for (int val : row) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}