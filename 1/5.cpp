#include <iostream>
#include <vector>
#include <thread>
#include <iomanip>
#include <random>
#include <chrono>
#include <cassert>

struct TreeNode {
    int value;
    TreeNode* left;
    TreeNode* right;

    TreeNode(int val) : value(val), left(nullptr), right(nullptr) {}
};

TreeNode* createTree(const std::vector<int>& arr, int index = 0) {
    if (index >= arr.size() || arr[index] == -1) {
        return nullptr;
    }

    TreeNode* node = new TreeNode(arr[index]);
    node->left = createTree(arr, 2 * index + 1);
    node->right = createTree(arr, 2 * index + 2);
    return node;
}

std::pair<long long, int> calculateSumAndCount(TreeNode* node) {
    if (node == nullptr) {
        return {0, 0};
    }

    auto left = calculateSumAndCount(node->left);
    auto right = calculateSumAndCount(node->right);

    long long sum = left.first + right.first + node->value;
    int count = left.second + right.second + 1;

    return {sum, count};
}

void deleteTree(TreeNode* node) {
    if (node == nullptr) return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

int main() {
    std::vector<std::vector<int>> testCases = {
        {1, 2, 3, 4, 5, -1, 6},
        {1, -1, 2, -1, -1, 3, -1},
        {100},
        {}
    };
    std::vector<std::pair<long long, double>> expectedResults = {
        {21, 3.5},
        {6, 2.0},
        {100, 100.0},
        {0, 0.0}
    };

    for (size_t i = 0; i < testCases.size(); ++i) {
        TreeNode* root = createTree(testCases[i]);

        auto [sum, count] = calculateSumAndCount(root);
        double average = (count > 0) ? static_cast<double>(sum) / count : 0.0;

        std::cout << "Tree: ";
        for (int val : testCases[i]) {
            std::cout << val << " ";
        }
        std::cout << "\nSum: " << sum << ", Average: " << average << "\n";

        assert(sum == expectedResults[i].first);
        assert(std::abs(average - expectedResults[i].second) < 0.001);

        deleteTree(root);
        std::cout << "\n";
    }

    return 0;
}