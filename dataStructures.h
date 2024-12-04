#ifndef HEAP_H
#define HEAP_H

#include <vector>
#include <utility>
#include <limits.h>
#include <algorithm>

typedef int vertex;

class Heap {
private:
    std::vector<std::pair<int, vertex>> heap; // Stores pairs of (distance, vertex)
    void heapify(int index, int size);
    void swap(int i, int j);
    
public:
    Heap() {}

    bool empty() const { return heap.empty(); }
    void insert_or_update(int dist, vertex v);
    void pop();
    std::pair<int, vertex> top() const;
    int size() const { return heap.size(); }
};

class UnionFind {
public:
    // Constructor initializes the disjoint set with `numElements` elements
    UnionFind(int numElements)
        : m_numElements(numElements), m_group(nullptr), m_groupSize(nullptr) {
        m_group = new int[numElements];
        m_groupSize = new int[numElements];

        // Initially, each element is its own leader (root of its own tree)
        for (int e = 0; e < numElements; e++) {
            m_group[e] = e;
            m_groupSize[e] = 1;
        }
    }

    // Destructor to clean up dynamically allocated memory
    ~UnionFind() {
        delete[] m_group;
        delete[] m_groupSize;
    }

    // Find the leader (root) of the set to which element `e` belongs
    int findE(int e) {
        // Path compression: Make every node on the path point directly to the root
        if (m_group[e] != e) {
            m_group[e] = findE(m_group[e]); // Recursively find the root and compress the path
        }
        return m_group[e];
    }

    // Union of two sets: the ones containing `e1` and `e2`
    void unionE(int e1, int e2) {
        int root1 = findE(e1);
        int root2 = findE(e2);

        if (root1 != root2) {
            // Union by size: attach the smaller tree under the larger one
            if (m_groupSize[root1] < m_groupSize[root2]) {
                m_group[root1] = root2;
                m_groupSize[root2] += m_groupSize[root1];
            } else {
                m_group[root2] = root1;
                m_groupSize[root1] += m_groupSize[root2];
            }
        }
    }

private:
    int m_numElements;   // Number of elements in the union-find structure
    int* m_group;        // Array to store the leader (root) of each element
    int* m_groupSize;    // Array to store the size of each tree (used for union by size)
};

#endif // HEAP_H
