#pragma once

#include <vector>
#include <unordered_map>

namespace Graph {

    // Definição da estrutura de dados do grafo ponderado
    struct Vertex {
        int id;
        // Outros atributos dos vértices (ex: coordenadas)
    };

    struct Edge {
        int source;
        int destination;
        double weight;
    };

    class WeightedGraph {
    public:
        void addVertex(const Vertex& vertex);
        void addEdge(const Edge& edge);
        // Outras operações para manipular o grafo
    private:
        std::vector<Vertex> vertices;
        std::vector<Edge> edges;
        std::unordered_map<int, std::vector<int>> adjacencyList;
    };

    // Declaração de funções relacionadas ao grafo
    void createGraph();
    double getCostBetweenVertices(int source, int destination);
    // Outras funções de consulta e manipulação do grafo
}