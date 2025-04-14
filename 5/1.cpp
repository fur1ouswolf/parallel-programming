#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <cmath>
#include <map>
#include <vector>

template <typename T>
struct Node {
    T data;
    std::shared_ptr<Node<T>> next;

    Node(T value) : data(value), next(nullptr) {}
};

template <typename T>
class LinkedList {
private:
    std::shared_ptr<Node<T>> head;
    std::shared_ptr<Node<T>> tail;
    int size;

public:
    LinkedList() : head(nullptr), tail(nullptr), size(0) {}

    void add(T value) {
        auto newNode = std::make_shared<Node<T>>(value);
        if (!head) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        size++;
    }

    std::shared_ptr<Node<T>> getHead() const {
        return head;
    }

    void setHead(std::shared_ptr<Node<T>> newHead) {
        head = newHead;
    }

    int getSize() const {
        return size;
    }

    void print() const {
        auto current = head;
        while (current) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }
};

template <typename T>
class Synchronizer {
private:
    std::mutex mutex;
    std::condition_variable cv;
    int pendingMerges;

public:
    Synchronizer(int totalMerges) : pendingMerges(totalMerges) {}

    void mergeDone() {
        std::lock_guard<std::mutex> lock(mutex);
        pendingMerges--;
        cv.notify_all();
    }

    void waitForAllMerges() {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this]() { return pendingMerges == 0; });
    }
};

template <typename T>
std::vector<LinkedList<T>> splitList(const LinkedList<T>& list) {
    std::vector<LinkedList<T>> parts(3);

    int totalSize = list.getSize();
    int partSize = totalSize / 3;
    int remaining = totalSize % 3;

    std::vector<int> sizes = {
        partSize + (remaining > 0 ? 1 : 0),
        partSize + (remaining > 1 ? 1 : 0),
        partSize
    };

    auto current = list.getHead();

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < sizes[i] && current; j++) {
            parts[i].add(current->data);
            current = current->next;
        }
    }

    return parts;
}

template <typename T>
LinkedList<T> mergeTwoLists(const LinkedList<T>& list1, const LinkedList<T>& list2) {
    LinkedList<T> result;
    auto l1 = list1.getHead();
    auto l2 = list2.getHead();

    while (l1 && l2) {
        if (l1->data <= l2->data) {
            result.add(l1->data);
            l1 = l1->next;
        } else {
            result.add(l2->data);
            l2 = l2->next;
        }
    }

    while (l1) {
        result.add(l1->data);
        l1 = l1->next;
    }

    while (l2) {
        result.add(l2->data);
        l2 = l2->next;
    }

    return result;
}

template <typename T>
LinkedList<T> mergeSort(LinkedList<T>& list) {
    if (list.getSize() <= 1) {
        return list;
    }

    std::vector<LinkedList<T>> parts = splitList(list);
    std::vector<LinkedList<T>> sortedParts(3);

    std::vector<std::thread> sortThreads;
    for (int i = 0; i < 3; i++) {
        sortThreads.emplace_back([&sortedParts, &parts, i]() {
            sortedParts[i] = mergeSort(parts[i]);
        });
    }

    for (auto& thread : sortThreads) {
        thread.join();
    }

    auto sync = std::make_shared<Synchronizer<T>>(1);

    LinkedList<T> intermediate;
    std::thread mergeThread([&sortedParts, &intermediate, &sync]() {
        intermediate = mergeTwoLists(sortedParts[0], sortedParts[1]);
        sync->mergeDone();
    });

    sync->waitForAllMerges();
    mergeThread.join();

    return mergeTwoLists(intermediate, sortedParts[2]);
}

template <typename T>
LinkedList<T> threeTapeMergeSort(LinkedList<T>& list) {
    return mergeSort(list);
}

int main1() {
    LinkedList<int> list;

    std::vector<int> values = {42, 15, 0, 30, 8, 21, 14, 9, 5, 11, 1, 2, 4, 3, 10, 11, 12, 13, 15, 14, 17, 7};
    for (int val : values) {
        list.add(val);
    }

    std::cout << "Original: ";
    list.print();

    LinkedList<int> sortedList = threeTapeMergeSort(list);

    std::cout << "Sorted: ";
    sortedList.print();

    return 0;
}