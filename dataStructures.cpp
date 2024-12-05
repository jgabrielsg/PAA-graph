#include "dataStructures.h"
#include <functional>  // Required for std::greater<>


void Heap::heapify(int index, int size) {
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    int smallest = index;

    if (left < size && heap[left].first < heap[smallest].first) {
        smallest = left;
    }
    if (right < size && heap[right].first < heap[smallest].first) {
        smallest = right;
    }

    if (smallest != index) {
        swap(index, smallest);
        heapify(smallest, size);
    }
}

void Heap::swap(int i, int j) {
    std::pair<int, vertex> temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
}

void Heap::insert_or_update(int dist, vertex v) {
    // Try to find if the vertex is already in the heap
    for (int i = 0; i < heap.size(); ++i) {
        if (heap[i].second == v) {
            // If found, update the distance
            if (dist < heap[i].first) {
                heap[i].first = dist;
                // Re-heapify to maintain heap property
                std::make_heap(heap.begin(), heap.end(), std::greater<>());  // Re-heapify
            }
            return;
        }
    }
    // If not found, insert the new vertex-distance pair
    heap.push_back({dist, v});
    std::push_heap(heap.begin(), heap.end(), std::greater<>()); // Min-heap based on distance
}


void Heap::pop() {
    std::pop_heap(heap.begin(), heap.end(), std::greater<>());
    heap.pop_back();
}

std::pair<int, vertex> Heap::top() const {
    return heap.front();
}