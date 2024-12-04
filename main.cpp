#include <iostream>
#include <fstream>
#include "external/json.hpp"
#include "Graph.h"

using json = nlohmann::json;

// Function to load the graph from the JSON file
void loadGraphFromJson(const std::string& filename, GraphAdjList& graph) {
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
        graph.addEdge(v1, v2);
    }
}

// Main function to test the graph reading
int main() {
    const std::string filename = "city_graph.json";
    int numVertices = 10;  // Replace with the actual number of nodes in your graph
    GraphAdjList graph(numVertices);

    // Load the graph from the JSON file
    loadGraphFromJson(filename, graph);

    // Print the graph to check if the edges are correctly loaded
    std::cout << "Graph edges:" << std::endl;
    graph.print();

    return 0;
}
