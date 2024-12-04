#include <iostream>
#include <fstream>
#include <climits>
#include <unordered_map>
#include "external/json.hpp"
#include "Graph.h"
#include "newMetro.h"  // Assuming this includes your Dijkstra and Kruskal algorithms

using json = nlohmann::json;

// Function to load the graph from the JSON file
void loadGraphFromJson(const std::string& filename, Graph& graph) {
    // Open the JSON file
    std::ifstream inputFile(filename);
    if (!inputFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    json j;
    inputFile >> j;

    // Create a map for node IDs to vertex indices
    std::unordered_map<std::string, vertex> nodeMap;
    int vertexIndex = 0;

    // Add nodes to the graph
    for (const auto& node : j["nodes"]) {
        nodeMap[node["id"]] = vertexIndex++;
    }

    // Add edges to the graph
    for (const auto& edge : j["edges"]) {
        vertex v1 = nodeMap[edge["from"]];
        vertex v2 = nodeMap[edge["to"]];
        int cost = edge["price_cost"];  // Ensure we're using 'price_cost' here
        graph.addEdge(v1, v2, cost);  // Assuming the Graph class handles edge addition with 'cost'
    }
}

// Main function to test the graph reading and algorithms
int main() {
    const std::string filename = "city_graph.json";
    int numVertices = 10;  // Replace with the actual number of nodes in your graph
    Graph graph(numVertices);

    // Load the graph from the JSON file
    loadGraphFromJson(filename, graph);

    // Print the graph to check if the edges are correctly loaded
    std::cout << "Graph edges:" << std::endl;
    graph.print();

    // Run Dijkstra's algorithm
    Dijkstra dijkstra;
    int* parent = new int[graph.getNumVertices()];
    int* distance = new int[graph.getNumVertices()];
    vertex startVertex = 0; // Assuming vertex 0 is the start
    dijkstra.cptDijkstraFast(startVertex, parent, distance, graph);

    std::cout << "\nDijkstra's Algorithm Result:" << std::endl;
    for (vertex v = 0; v < graph.getNumVertices(); ++v) {
        std::cout << "Distance to vertex " << v << ": " << distance[v] << ", Parent: " << parent[v] << std::endl;
    }

    // Run Kruskal's algorithm for MST
    std::vector<Edge*> mstEdges;
    Kruskal kruskal;
    kruskal.mstKruskalFast(mstEdges, graph);

    std::cout << "\nKruskal's Algorithm Result (Minimum Spanning Tree):" << std::endl;
    for (Edge* edge : mstEdges) {
        std::cout << "Edge " << edge->v1() << " - " << edge->v2() << " with cost: " << edge->cost() << std::endl;
        delete edge;  // Clean up dynamically allocated memory
    }

    // Clean up dynamically allocated memory
    delete[] parent;
    delete[] distance;

    return 0;
}
