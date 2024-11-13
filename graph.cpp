#include "graph.h"

namespace Graph {

    void WeightedGraph::addVertex(const Vertex& vertex) {
        vertices.push_back(vertex);
        adjacencyList[vertex.id] = std::vector<int>();
    }

    void WeightedGraph::addEdge(const Edge& edge) {
        edges.push_back(edge);
        adjacencyList[edge.source].push_back(edge.destination);
        adjacencyList[edge.destination].push_back(edge.source);
    }

    double getCostBetweenVertices(int source, int destination) {
        // Implementação do cálculo do custo entre dois vértices
        // Pode envolver algoritmos de caminho mínimo, por exemplo
        return 0.0;
    }

    void createGraph() {
        // Implementação da criação do grafo ponderado
        // Adicionando vértices e arestas
    }

}