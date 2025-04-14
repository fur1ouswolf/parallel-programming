#include <iostream>
#include <vector>
#include <thread>
#include <condition_variable>
#include <algorithm>
#include <ctime>
#include <string>

double f(double x) {
    return x * x;
}

double calculateSegmentArea(double start, double end, int m) {
    double width = (end - start) / m;
    double area = 0;
    for (int i = 0; i < m; ++i) {
    double x = start + i * width;
    area += f(x) * width;
    }
    return area;
}

void calculateArea(double start, double end, int m, std::mutex& mutex, std::vector<double>& segmentAreas) {
    double area = calculateSegmentArea(start, end, m);
    std::lock_guard<std::mutex> lock(mutex);
    segmentAreas.push_back(area);
}

int main3() {
    double a, b;
    std::cout << "Начало отрезка (a): ";
    std::cin >> a;
    std::cout << "Конец отрезка (b): ";
    std::cin >> b;

    int n, m;
    std::cout << "Количество сегментов (n): ";
    std::cin >> n;
    std::cout << "Количество участков в сегменте (m): ";
    std::cin >> m;

    if (n <= 0 || m <= 0 || a >= b) {
        std::cerr << "Incorrect input!" << std::endl;
        return 1;
    }

    std::vector<std::thread> threads;
    std::vector<double> segmentAreas;
    std::mutex areaMutex;

    double segmentWidth = (b - a) / n;

    for (int i = 0; i < n; ++i) {
        double start = a + i * segmentWidth;
        double end = a + (i + 1) * segmentWidth;
        threads.emplace_back(calculateArea, start, end, m, std::ref(areaMutex), std::ref(segmentAreas));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    double totalArea = 0;
    for (double area : segmentAreas) {
        totalArea += area;
    }

    std::cout << "\nПриближенная площадь криволинейной трапеции: " << totalArea << std::endl;

    return 0;
}