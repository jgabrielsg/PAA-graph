#include <random>
#include "bus.h"
#include "Graph.h"
#include "newMetro.h"
#include "dataStructures.h"
#include <unordered_set>
#include <unordered_map>
#include <climits>
#include <algorithm>
#include <vector>
#include <iostream>

double calculateEdgeWeight(const Edge& edge) {
    double weight = -edge.num_commercial() * 2.0 - edge.num_touristic() * 3.0 +
                     edge.num_residencial() * 1.5 + edge.num_industrial() * 1.0;
    return weight;
}

std::vector<vertex> findHamiltonianCycle(Graph& graph) {
    int n = graph.getNumVertices();
    std::vector<bool> visited(n, false);
    std::vector<vertex> cycle;

    vertex start = 0;  // Começa de qualquer vértice (aqui, 0)
    cycle.push_back(start);
    visited[start] = true;

    vertex current = start;
    for (int i = 0; i < n - 1; ++i) {
        double minWeight = std::numeric_limits<double>::max();
        vertex next = -1;

        Edge* edge = graph.getEdges(current);
        while (edge) {
            vertex neighbor = edge->otherVertex(current);
            if (!visited[neighbor]) {
                double weight = calculateEdgeWeight(*edge);
                if (weight < minWeight) {
                    minWeight = weight;
                    next = neighbor;
                }
            }
            edge = edge->next();
        }

        if (next == -1) break;  // Nenhum vizinho disponível
        cycle.push_back(next);
        visited[next] = true;
        current = next;
    }

    return cycle;
}

void designBusRoute(Graph& graph) {
    // Calcula o ciclo hamiltoniano
    std::vector<vertex> cycle = findHamiltonianCycle(graph);

    // Exibe o resultado no formato desejado
    std::cout << "[";
    for (size_t i = 0; i < cycle.size() - 1; i++) {
        std::string node1 = graph.getNodeId(cycle[i]);
        std::string node2 = graph.getNodeId(cycle[i + 1]);

        // Imprime os pares de nós no formato ('node_1', 'node_2')
        std::cout << "('" << node1 << "','" << node2 << "'),";
    }

    int numVertices = graph.getNumVertices();

    std::vector<int> distancia(numVertices, INT_MAX); 
    std::vector<vertex> parent(numVertices, -1);

    vertex current = cycle[cycle.size() - 1];
    vertex v1 = cycle[0];

    Dijkstra::cptDijkstraFast(v1, parent.data(), distancia.data(), graph);

    while (current != v1 && current != -1) {
        vertex p = parent[current];

        Edge* originalEdge = graph.getEdges(p);
        bool edgeFound = false;
        while (originalEdge) {
            if (originalEdge->otherVertex(p) == current) {
                std::cout << "('" << graph.getNodeId(p) << "','" << graph.getNodeId(current) << "'),";
                break;
            }
            originalEdge = originalEdge->next();
        }

        current = p;
    }

    std::cout << "]" << std::endl;
}