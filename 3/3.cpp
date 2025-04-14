#include <numeric>
#include <iostream>
#include <thread>
#include <vector>
#include <condition_variable>
#include <random>

class CMonitor {
private:
    std::vector<bool> occupiedRows;
    std::mutex tableMutex;
    std::condition_variable cv;

public:
    CMonitor(int size) : occupiedRows(size, false) {}

    void occupyRow(int rowNumber) {
        std::unique_lock<std::mutex> lock(tableMutex);
        cv.wait(lock, [this, rowNumber]{ return !occupiedRows[rowNumber]; });
        occupiedRows[rowNumber] = true;
    }

    void freeRow(int rowNumber) {
        std::unique_lock<std::mutex> lock(tableMutex);
        occupiedRows[rowNumber] = false;
        cv.notify_all();
    }
};

class HashTable {
private:
    std::vector<int> table;
    CMonitor* monitor;
    int k;

public:
    HashTable(int size, int k, CMonitor* monitor)
        : table(size, 0), k(k), monitor(monitor) {}

    void insert(int value) {
        int index = hash(value);
        monitor->occupyRow(index);
        table[index] = value;
        monitor->freeRow(index);
    }

    int hash(int x) {
        return x % k;
    }

    void printTable() {
        for (size_t i = 0; i < table.size(); ++i) {
            std::cout << "Bucket " << i << ": ";
            for (size_t j = 0; j < table.size(); ++j) {
                std::cout << table[j] << " ";
            }
            std::cout << std::endl;
        }
    }
};

void generateAndInsert(HashTable& table, int count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000);

    for (int i = 0; i < count; ++i) {
        int value = dis(gen);
        table.insert(value);
    }
}

int main() {
    int tableSize = 10;
    int k = 10;
    int numThreads = 4;
    int insertionsPerThread = 10;

    CMonitor monitor(tableSize);
    HashTable table(tableSize, k, &monitor);
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(generateAndInsert, std::ref(table), insertionsPerThread);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "\nFinal hash table:\n";
    table.printTable();

    return 0;
}