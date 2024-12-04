#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"
#include "dataStructures.h"
#include <vector>
#include <string>
#include <unordered_map>

// Function declarations for Dijkstra's algorithm
void cptDijkstraFast(const Graph& graph, const std::string& v0_id, std::vector<std::string>& parent, std::vector<int>& distance);

// UnionFind class for Kruskal's MST algorithm
class UnionFind {
public:
    UnionFind(int numElements);
    ~UnionFind();
    
    int findE(int e);
    void unionE(int e1, int e2);

private:
    int m_numElements;
    int* m_group;
    int* m_groupSize;
};

// Kruskal's Minimum Spanning Tree (MST) algorithm
void mstKruskalFast(const Graph& graph, std::vector<Edge>& mstEdges);

void findStations(const Graph& graph, std::unordered_map<std::string, std::string>& result);

#endif // DIJKSTRA_H
