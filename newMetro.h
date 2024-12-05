#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "Graph.h"
#include "dataStructures.h"

std::vector<std::vector<vertex>> criarRegioes(Graph &g);
std::vector<Edge*> escavacaoMetro(Graph& graph);

class Dijkstra {
public:
    static void cptDijkstraFast(vertex v0, vertex* parent, int* distance, Graph& graph);
};

class Kruskal {
public:
    static void mstKruskalFast(std::vector<Edge*>& mstEdges, Graph& graph);
};

#endif // DIJKSTRA_H