#include "newMetro.h"
#include <climits>

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
                int cost = edge->cost(); // Get the cost from the edge
                if (distance[v1] + cost < distance[v2]) {
                    parent[v2] = v1;
                    distance[v2] = distance[v1] + cost;
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
                edges.push_back(Edge(v1, v2, edge->cost())); // Create edge and store it
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
            mstEdges.push_back(new Edge(edge.v1(), edge.v2(), edge.cost())); // Add edge to MST
        }
    }
}

