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
    // Open the JSON file and parse it
    std::ifstream file(filename);
    json graphData;
    file >> graphData;

    // Load nodes and edges
    auto nodes = graphData["nodes"];
    auto edges = graphData["edges"];

    // Map node ids to integer indices
    std::unordered_map<std::string, vertex> nodeMap;
    vertex nodeId = 0;

    // Add nodes to the graph
    for (const auto& node : nodes) {
        std::string nodeIdStr = node["id"];
        
        // Store node ID and region before incrementing
        graph.setNodeId(nodeId, nodeIdStr); // Set node ID for the current vertex
        graph.setRegion(nodeIdStr, node["region"]);  // Store the region
        
        nodeMap[nodeIdStr] = nodeId;  // Map node ID to current vertex index
        nodeId++;  // Increment for the next node
    }



    // Add edges to the graph
    for (const auto& edge : edges) {
        std::string fromNodeId = edge["from"];
        std::string toNodeId = edge["to"];
        
        // Get corresponding vertex indices for the node IDs
        vertex v1 = nodeMap[fromNodeId];
        vertex v2 = nodeMap[toNodeId];

        // Read edge properties
        int cost = edge["excavation_cost"];  // Assuming 'price_cost' is the cost
        int distance = edge["distance"];

        // Add the edge to the graph in both directions (undirected graph)
        graph.addEdge(v1, v2, cost, distance);
        graph.addEdge(v2, v1, cost, distance);
    }
}


int main() {
    const std::string filename = "city_graph.json";

    // First, parse the JSON to count the number of nodes
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return 1;
    }
    
    json graphData;
    file >> graphData;
    auto nodes = graphData["nodes"];
    int numVertices = nodes.size();  // Replace with the actual number of nodes in your graph
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
        // Retrieve the node ID based on the vertex index
        std::string nodeId = graph.getNodeId(v); 
        
        std::cout << "Distance to vertex " << v << ": " 
                << distance[v] << ", Parent: " 
                << parent[v] << ", Region: " 
                << graph.getRegion(nodeId) << std::endl;
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
