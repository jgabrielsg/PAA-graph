#include "newMetro.h"
#include <climits>
#include <vector>
#include <algorithm>
#include "Graph.h"
#include <limits.h>

void Dijkstra::cptDijkstraFast(vertex v0, vertex* parent, int* distance, Graph& graph) {
    std::vector<bool> checked(graph.getNumVertices(), false);
    Heap heap; // Create the heap
    
    // Initialize arrays
    for (vertex v = 0; v < graph.getNumVertices(); v++) {
        parent[v] = -1;
        distance[v] = INT_MAX;
        checked[v] = false;
    }
    parent[v0] = v0;
    distance[v0] = 0;
    heap.insert_or_update(distance[v0], v0); // Add starting vertex to heap

    while (!heap.empty()) {
        vertex v1 = heap.top().second; // Get vertex with the smallest distance
        heap.pop(); // Remove it from the heap
        
        if (distance[v1] == INT_MAX) { break; } // Exit if remaining vertices are unreachable
        
        std::cout << "Processing vertex " << v1 << " with distance " << distance[v1] << std::endl;

        // Iterate through all edges connected to v1
        Edge* edge = graph.getEdges(v1); // Access edges from the Graph
        while (edge) {
            vertex v2 = edge->otherVertex(v1); // Get the other vertex of the edge
            std::cout << "Checking edge (" << v1 << ", " << v2 << ") with distance " << edge->distance() << std::endl;
            if (!checked[v2]) {
                int current_distance = edge->distance(); // Get the cost from the edge
                if (distance[v1] + current_distance < distance[v2]) {
                    parent[v2] = v1;
                    distance[v2] = distance[v1] + current_distance;
                    heap.insert_or_update(distance[v2], v2); // Update heap with new distance
                }
            }
            edge = edge->next(); // Move to the next edge
        }
        checked[v1] = true; // Mark the vertex as checked
    }
}

void Kruskal::mstKruskalFast(std::vector<Edge*>& mstEdges, Graph& graph) {
    std::vector<Edge> edges;

    // Collect all edges from the graph
    for (vertex v1 = 0; v1 < graph.getNumVertices(); v1++) {
        Edge* edge = graph.getEdges(v1);
        while (edge) {
            vertex v2 = edge->otherVertex(v1);
            if (v1 < v2) { // Avoid duplicate edges (v1 < v2 ensures each edge is considered once)
                edges.push_back(Edge(v1, v2, edge->cost(), edge->distance())); // Create edge and store it
            }
            edge = edge->next(); // Move to next edge
        }
    }

    // Sort edges by cost
    std::sort(edges.begin(), edges.end(), [](const Edge& e1, const Edge& e2) {
        return e1.cost() < e2.cost();
    });

    UnionFind uf(graph.getNumVertices());
    int currentEdge = 0;

    // Kruskal's algorithm
    for (const Edge& edge : edges) {
        vertex leaderV1 = uf.findE(edge.v1());
        vertex leaderV2 = uf.findE(edge.v2());
        if (leaderV1 != leaderV2) {
            uf.unionE(leaderV1, leaderV2); // Union the sets
            mstEdges.push_back(new Edge(edge.v1(), edge.v2(), edge.cost(), edge.distance(), nullptr));
            std::cout << "Edge added to MST: (" << edge.v1() << ", " << edge.v2() << ") with cost " << edge.cost() << std::endl;
        }
    }
}

std::vector<Edge*> escavacaoMetro(Graph& graph, const std::vector<std::vector<vertex>>& regioes) {
    std::vector<vertex> estacoes;
    std::vector<Edge*> solucao;  
    int numVertices = graph.getNumVertices();

    for (const auto& regiao : regioes) {
        int minMaxDist = INT_MAX;
        vertex c_min = -1;

        for (vertex v : regiao) {
            std::vector<int> distancia(numVertices, INT_MAX); 
            std::vector<vertex> parent(numVertices, -1);      
            
            Dijkstra::cptDijkstraFast(v, parent.data(), distancia.data(), graph);

            int maxDist = 0;
            for (vertex r : regiao) {
                maxDist = std::max(maxDist, distancia[r]);
            }

            if (maxDist < minMaxDist) {
                minMaxDist = maxDist;
                c_min = v;
            }
        }

        estacoes.push_back(c_min);
        std::cout << "estacoes: " << c_min << std::endl;
    }

    // construindo um subgrafo
    Graph subgrafo(numVertices);
    for (vertex v1 : estacoes) {
        Edge* edge = graph.getEdges(v1);
        while (edge) {
            vertex v2 = edge->otherVertex(v1);
            if (std::find(estacoes.begin(), estacoes.end(), v2) != estacoes.end()) {
                subgrafo.addEdge(v1, v2, edge->cost(), edge->distance());
            }
            edge = edge->next();
        }
    }

    std::vector<Edge*> arestasOrdenadas;
    for (vertex v : estacoes) {
        Edge* edge = subgrafo.getEdges(v);
        while (edge) {
            arestasOrdenadas.push_back(edge);
            edge = edge->next();
        }
    }
    std::sort(arestasOrdenadas.begin(), arestasOrdenadas.end(), [](Edge* e1, Edge* e2) {
        return e1->cost() < e2->cost();
    });

    Kruskal::mstKruskalFast(solucao, subgrafo);

    return solucao;
}