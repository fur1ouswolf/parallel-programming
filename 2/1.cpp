#include <iostream>
#include <vector>
#include <thread>
#include <condition_variable>
#include <algorithm>
#include <ctime>
#include <string>

void createRow(std::vector<std::vector<double>>& matrix, int row, std::mutex& mutex, std::condition_variable& cv) {
    std::lock_guard<std::mutex> lock(mutex);
    for (int j = 0; j < matrix[0].size(); ++j) {
        matrix[row][j] = static_cast<double>(rand()) / RAND_MAX * 100;
    }
    cv.notify_one();
}

void sumRow(const std::vector<std::vector<double>>& matrix, int row, std::vector<double>& rowSums, std::mutex& mutex, std::condition_variable& cv) {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [&]{ return matrix[row][0] != 0; });
    double sum = 0;
    for (double num : matrix[row]) {
        sum += num;
    }
    rowSums[row] = sum;
}

int main1() {
    srand(static_cast<unsigned int>(time(0)));

    int n, m;
    std::cout << "N: ";
    std::cin >> n;
    std::cout << "M: ";
    std::cin >> m;

    std::vector<std::vector<double>> matrix(n, std::vector<double>(m));

    std::vector<double> rowSums(n);

    std::mutex matrixMutex;
    std::condition_variable cv;

    std::vector<std::thread> createThreads;
    std::vector<std::thread> sumThreads;

    for (int i = 0; i < n; ++i) {
        createThreads.emplace_back(createRow, std::ref(matrix), i, std::ref(matrixMutex), std::ref(cv));
    }

    for (int i = 0; i < n; ++i) {
        sumThreads.emplace_back(sumRow, std::cref(matrix), i, std::ref(rowSums), std::ref(matrixMutex), std::ref(cv));
    }

    for (auto& thread : createThreads) {
        thread.join();
    }
    for (auto& thread : sumThreads) {
        thread.join();
    }

    std::cout << "\nOriginal:" << std::endl;
    for (const auto& row : matrix) {
        for (double num : row) {
            std::cout << num << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "\nSum by row:" << std::endl;
    for (int i = 0; i < n; ++i) {
        std::cout << "Row " << i << ": " << rowSums[i] << std::endl;
    }

    double totalSum = 0;
    for (double sum : rowSums) {
        totalSum += sum;
    }

    std::cout << "\nTotal sum:: " << totalSum << std::endl;

    return 0;
}