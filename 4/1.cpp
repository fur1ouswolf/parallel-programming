#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <thread>
#include <mutex>
#include <algorithm>
#include <condition_variable>
#include <string>

class PrintPool {
private:
    int capacity;
    std::string buffer;
    std::mutex mtx;
    std::condition_variable cv_produce, cv_consume;
    bool stop = false;

public:
    PrintPool(int size) : capacity(size) {}

    bool write_to_pool(const std::string& text) {
        std::unique_lock<std::mutex> lock(mtx);
        while (buffer.size() + text.size() > capacity) {
            cv_produce.wait(lock);
        }
        buffer += text;
        cv_consume.notify_one();
        return true;
    }

    std::string read_from_pool() {
        std::unique_lock<std::mutex> lock(mtx);
        while (buffer.empty() && !stop) {
            cv_consume.wait(lock);
        }
        if (stop && buffer.empty()) return "";
        std::string output = buffer;
        buffer.clear();
        cv_produce.notify_all();
        return output;
    }

    void stop_processing() {
        std::lock_guard<std::mutex> lock(mtx);
        stop = true;
        cv_consume.notify_all();
    }
};

void print_worker(PrintPool& pool, const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Open file error: " << filename << std::endl;
        return;
    }
    std::string line, content;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    file.close();
    pool.write_to_pool(content);
}

void manager(PrintPool& pool, const std::string& output_filename) {
    std::ofstream output_file(output_filename, std::ios::app);
    if (!output_file) {
        std::cerr << "Open file error: " << std::endl;
        return;
    }
    while (true) {
        std::string text = pool.read_from_pool();
        if (text.empty()) break;
        output_file << text;
        output_file.flush();
    }
    output_file.close();
}

int main() {
    int pool_size, num_workers;
    std::cout << "Write pool size (in bytes): ";
    std::cin >> pool_size;
    std::cout << "Write num of workers: ";
    std::cin >> num_workers;

    PrintPool pool(pool_size);
    std::vector<std::thread> workers;
    std::vector<std::string> filenames = {"file1.txt", "file2.txt", "file3.txt"};

    std::thread manager_thread(manager, std::ref(pool), "output.txt");

    for (int i = 0; i < num_workers && i < filenames.size(); ++i) {
        workers.emplace_back(print_worker, std::ref(pool), filenames[i]);
    }

    for (auto& worker : workers) {
        worker.join();
    }

    pool.stop_processing();
    manager_thread.join();

    return 0;
}