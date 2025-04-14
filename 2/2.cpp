#include <iostream>
#include <vector>
#include <thread>
#include <condition_variable>
#include <algorithm>
#include <ctime>
#include <string>

void calculateChecksum(const std::string& text, int threadId, int threadCount, std::mutex& mutex, std::vector<int>& threadSums) {
    int sum = 0;
    for (size_t i = threadId; i < text.size(); i += threadCount) {
        sum += text[i];
    }
    std::lock_guard<std::mutex> lock(mutex);
    threadSums[threadId] = sum % 256;
}

int main2() {
    std::string text;
    std::cout << "Текст: ";
    std::getline(std::cin, text);

    int k;
    std::cout << "Количество потоков (меньше длины текста): ";
    std::cin >> k;

    if (k <= 0 || k >= static_cast<int>(text.size())) {
        std::cerr << "Incorrect input!" << std::endl;
        return 1;
    }

    std::vector<std::thread> threads;
    std::vector<int> threadSums(k);
    std::mutex sumMutex;

    for (int i = 0; i < k; ++i) {
        threads.emplace_back(calculateChecksum, std::cref(text), i, k, std::ref(sumMutex), std::ref(threadSums));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    int checksum = 0;
    for (int sum : threadSums) {
        checksum += sum;
    }
    checksum %= 256;

    std::cout << "\nChecksum: " << checksum << std::endl;

    return 0;
}