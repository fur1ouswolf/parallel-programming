#include <iostream>
#include <vector>
#include <thread>
#include <iomanip>
#include <random>
#include <chrono>
#include <cassert>

void countSort(std::vector<double>& row) {
    int maxVal = *std::max_element(row.begin(), row.end());
    int minVal = *std::min_element(row.begin(), row.end());
    std::vector<double> count(maxVal - minVal + 1, 0);

    for (double val : row) {
        count[val - minVal]++;
    }

    int index = 0;
    for (double i = maxVal; i >= minVal; i -= 1.0) {
        while (count[i - minVal] > 0) {
            row[index++] = i;
            count[i - minVal]--;
        }
    }
}

int main() {
    int n = 3, m = 4;
    std::vector<std::vector<double>> matrix = {
        {3.5, 1.2, 4.7, 4.7},
        {6.3, 5.1, 7.9, 8.2},
        {9.0, 10.5, 11.3, 12.1}
    };

    for (auto& row : matrix) {
        countSort(row);
    }

    for (const auto& row : matrix) {
        for (double val : row) {
            std::cout << std::fixed << std::setprecision(1) << val << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}