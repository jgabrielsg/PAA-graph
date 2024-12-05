#include "newMetro.h"
#include <climits>
#include <vector>
#include <algorithm>
#include "Graph.h"
#include <limits.h>
#include <tuple>

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

        // Iterate through all edges connected to v1
        Edge* edge = graph.getEdges(v1); // Access edges from the Graph
        while (edge) {
            vertex v2 = edge->otherVertex(v1); // Get the other vertex of the edge
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
                edges.push_back(Edge(v1, v2, edge->cost(), edge->distance(), edge->transport_type(), edge->max_speed(), edge->price_cost(), edge->time_cost(), edge->num_residencial(), edge->num_commercial(), edge->num_touristic(), edge->num_industrial(), edge->bus_preference())); // Create edge and store it
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
            mstEdges.push_back(new Edge(edge.v1(), edge.v2(), edge.cost(), edge.distance(), edge.transport_type(), edge.max_speed(), edge.price_cost(), edge.time_cost(), edge.num_residencial(), edge.num_commercial(), edge.num_touristic(), edge.num_industrial(), edge.bus_preference(), nullptr));
        }
    }
}

std::vector<std::vector<vertex>> criarRegioes(Graph& graph) {
    std::map<int, std::vector<vertex>> mapaRegioes;
    int numVertices = graph.getNumVertices();

    // Agrupar vértices pelo valor de nodeId
    for (int v = 0; v < numVertices; ++v) {
        std::string nodeId = graph.getNodeId(v);
        mapaRegioes[graph.getRegion(nodeId)].push_back(v);
    }

    // Converter o mapa em um vetor de vetores
    std::vector<std::vector<vertex>> regioes;
    for (const auto& [id, vertices] : mapaRegioes) {
        regioes.push_back(vertices);
    }

    return regioes;
}

std::tuple<std::vector<Edge*>, int, std::vector<vertex>> escavacaoMetro(Graph& graph) {
    std::vector<vertex> estacoes;
    std::vector<Edge*> solucao;  
    std::vector<std::vector<vertex>> regioes;

    regioes = criarRegioes(graph);
    int numVertices = graph.getNumVertices();

    std::unordered_map<vertex, std::vector<vertex>> parentMap;

    for (const auto& regiao : regioes) {
        std::vector<vertex> bestParent;
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
                bestParent = parent;
                c_min = v;
            }
        }

        estacoes.push_back(c_min);
        parentMap[c_min] = bestParent;
    }

    // construindo um subgrafo
    Graph subgrafo(numVertices);

    for (vertex v1 : estacoes) {
        for (vertex v2 : estacoes) {
            if (v1 == v2) continue; // Skip if both vertices are the same

            // Retrieve the parent array for station v1
            auto it = parentMap.find(v1);
            if (it == parentMap.end()) {
                std::cerr << "No parent array found for station " << v1 << std::endl;
                continue;
            }
            const std::vector<vertex>& parent = it->second;

            // Traverse from v2 back to v1 using the parent array
            vertex current = v2;
            while (current != v1 && current != -1) {
                vertex p = parent[current];
                if (p == -1) {
                    std::cerr << "No path from " << v1 << " to " << v2 << std::endl;
                    break;
                }

                // Add the edge between p and current to the subgrafo
                // Retrieve the original edge's cost and distance from the main graph
                Edge* originalEdge = graph.getEdges(p);
                bool edgeFound = false;
                while (originalEdge) {
                    if (originalEdge->otherVertex(p) == current) {
                        subgrafo.addEdge(p, current, originalEdge->cost(), originalEdge->distance(), originalEdge->transport_type(), originalEdge->max_speed(), originalEdge->price_cost(), originalEdge->time_cost(), originalEdge->num_residencial(), originalEdge->num_commercial(), originalEdge->num_touristic(), originalEdge->num_industrial(), originalEdge->bus_preference());
                        edgeFound = true;
                        break;
                    }
                    originalEdge = originalEdge->next();
                }

                if (!edgeFound) {
                    std::cerr << "Edge not found between " << p << " and " << current << std::endl;
                    break;
                }

                current = p; // Move to the parent vertex
            }
        }
    }

    int TotalCost = 0;
    std::vector<Edge*> arestasOrdenadas;
    for (vertex v : estacoes) {

        Edge* edge = subgrafo.getEdges(v);

        while (edge) {
            arestasOrdenadas.push_back(edge);
            TotalCost += edge->cost();
            edge = edge->next();
        }
    }

    std::sort(arestasOrdenadas.begin(), arestasOrdenadas.end(), [](Edge* e1, Edge* e2) {
        return e1->cost() < e2->cost();
    });

    Kruskal::mstKruskalFast(solucao, subgrafo);

    return std::make_tuple(solucao, TotalCost, estacoes);
}