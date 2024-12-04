#ifndef HEAP_H
#define HEAP_H

#include <vector>
#include <utility> // for std::pair

class Heap {
public:
    // Constructor (optional but can be added for clarity)
    Heap() = default;

    // Inserts or updates the heap with a distance and vertex pair
    void insert_or_update(int distance, int vertex);

    // Returns whether the heap is empty
    bool empty() const;

    // Returns the top (minimum) element from the heap
    std::pair<int, int> top() const;

    // Removes the top element from the heap
    void pop();

private:
    // Vector to store the heap (min-heap)
    std::vector<std::pair<int, int>> heap;

    // Builds the heap from the array of pairs (distance, vertex)
    void buildHeap();

    // Maintains the heap property for a specific index
    void heapify(int i);
};

#endif // HEAP_H
