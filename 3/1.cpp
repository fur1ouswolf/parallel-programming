#include <numeric>
#include <iostream>
#include <thread>
#include <vector>
#include <condition_variable>
#include <random>

// БЕЗ ВНЯТНОГО ТЗ - РЕЗУЛЬТАТ ХЗ

void dot_product(const std::vector<int>& vec1, const std::vector<int>& vec2, int& result) {
    result = std::inner_product(vec1.begin(), vec1.end(), vec2.begin(), 0);
}

int main() {
    int n;
    std::cout << "Vector size: ";
    std::cin >> n;

    std::vector<int> vec1(n), vec2(n);
    std::cout << "First vector elements: ";
    for (int& x : vec1) {
        std::cin >> x;
    }

    std::cout << "Second vector elements: ";
    for (int& x : vec2) {
        std::cin >> x;
    }

    int result = 0;
    std::thread t(dot_product, std::ref(vec1), std::ref(vec2), std::ref(result));
    t.join();

    std::cout << "Scalar product: " << result << std::endl;
    return 0;
}