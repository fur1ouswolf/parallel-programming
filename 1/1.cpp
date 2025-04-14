#include <iostream>
#include <vector>
#include <thread>
#include <iomanip>
#include <random>
#include <chrono>
#include <cassert>

void createRow(std::vector<double>& row, int size) {
    row.reserve(size);
    for (int i = 0; i < size; ++i) {
        row.emplace_back(rand() % 10);
    }
}

int main() {
    int n = 5, m = 5;
    std::vector<std::vector<double>> matrix;
    matrix.reserve(n);

    std::vector<std::thread> threads;
    for (int i = 0; i < n; ++i) {
        matrix.emplace_back(std::vector<double>());
        threads.emplace_back(createRow, std::ref(matrix[i]), m);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    for (const auto& row : matrix) {
        for (double val : row) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}