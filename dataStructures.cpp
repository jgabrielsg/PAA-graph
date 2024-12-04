#include "dataStructures.h"

void Heap::insert_or_update(int distance, int vertex) {
    heap.push_back({distance, vertex});
    buildHeap();  // Rebuild heap after insertion
}

bool Heap::empty() const {
    return heap.empty();
}

std::pair<int, int> Heap::top() const {
    return heap.front();  // The first element is always the minimum
}

void Heap::pop() {
    if (!empty()) {
        std::swap(heap.front(), heap.back());
        heap.pop_back();
        heapify(0);  // Rebuild heap after pop
    }
}

void Heap::buildHeap() {
    for (int i = (heap.size() / 2 - 1); i >= 0; i--) {
        heapify(i);
    }
}

void Heap::heapify(int i) {
    int n = heap.size();
    int inx = i;
    int leftInx = 2 * i + 1;
    int rightInx = 2 * i + 2;

    if (leftInx < n && heap[leftInx].first < heap[inx].first) {
        inx = leftInx;
    }
    if (rightInx < n && heap[rightInx].first < heap[inx].first) {
        inx = rightInx;
    }
    if (inx != i) {
        std::swap(heap[i], heap[inx]);
        heapify(inx);
    }
}
