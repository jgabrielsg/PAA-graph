#ifndef NEWMETRO_H
#define NEWMETRO_H

#include "dataStructures.h"
#include "graph.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <climits>

// Forward declarations of data structures
class Node;
class Edge;
class Region;
class UnionFind;

// The UnionFind data structure for Kruskal's algorithm
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

// Dijkstra's algorithm for shortest paths (fast implementation)
void cptDijkstraFast(const Graph&, const std::string&, std::vector<std::string>&, std::vector<int>&);

// Kruskal's Minimum Spanning Tree algorithm (MST)
void mstKruskalFast(const Graph&, std::vector<Edge>&);

// Method for finding the best stations in the graph
void findStations(const Graph&, std::unordered_map<int, std::string>&);

// Method for finding the best stations (updated version)
void findBestStations(const Graph&, std::unordered_map<int, std::string>& );

// Create a graph from the best stations
void createGraphFromBestStations(
    const std::unordered_map<int, std::string>&,
    Graph& , Graph& );

#endif // NEWMETRO_H
