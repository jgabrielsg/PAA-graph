#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "Graph.h"
#include "dataStructures.h"

class Dijkstra {
public:
    static void cptDijkstraFast(vertex v0, vertex* parent, int* distance, Graph& graph);
};

class Kruskal {
public:
    static void mstKruskalFast(std::vector<Edge*>& mstEdges, Graph& graph);
};

#endif // DIJKSTRA_H
