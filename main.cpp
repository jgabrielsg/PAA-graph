#include <iostream>
#include <fstream>
#include <climits>
#include <unordered_map>
#include "external/json.hpp"
#include "graph.h"
#include "newMetro.h"
#include "bus.h"
#include <tuple>
#include <fstream>

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
        std::string transport_type = edge["transport_type"];
        double max_speed = edge["max_speed"];
        double price_cost = edge["price_cost"];
        double time_cost = edge["time_cost"];
        int num_residencial = edge["num_residencial"];
        int num_commercial = edge["num_commercial"];
        int num_touristic = edge["num_touristic"];
        int num_industrial = edge["num_industrial"];
        int bus_preference = edge["bus_preference"];

        // Add the edge to the graph in both directions (undirected graph)
        graph.addEdge(v1, v2, cost, distance, transport_type, max_speed, price_cost, time_cost, num_residencial, num_commercial, num_touristic, num_industrial, bus_preference);
        graph.addEdge(v2, v1, cost, distance, transport_type, max_speed, price_cost, time_cost, num_residencial, num_commercial, num_touristic, num_industrial, bus_preference);
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
    // std::cout << "Graph edges:" << std::endl;
    // graph.print();

    std::cout << "Iniciando escavacaoMetro..." << std::endl;
    
    std::tuple<std::vector<Edge*>, int> result = escavacaoMetro(graph);
    std::vector<Edge*> mst = std::get<0>(result);
    int totalCost = std::get<1>(result);

    std::cout << "escavacaoMetro finalizado!" << std::endl;

    // std::cout << "[";
    // for (const auto& edge : mst) {
    //     if (edge) {
    //         std::cout << "('" << graph.getNodeId(edge->v1()) << "','" << graph.getNodeId(edge->v2()) << "'),";
    //     }
    // }
    // std::cout << "]" << std::endl;

    std::ofstream outFile("station_edges.txt");
    if (!outFile.is_open()) {
        std::cerr << "Falha ao abrir o arquivo para escrita." << std::endl;
        return 1;
    }

    // Escreve as edges no arquivo
    outFile << "[";
    bool first = true;
    for (const auto& edge : mst) {
        if (edge) {
            if (!first) {
                outFile << ",";
            }
            outFile << "('" << graph.getNodeId(edge->v1()) << "','" << graph.getNodeId(edge->v2()) << "')";
            first = false;
        }
    }
    outFile << "]" << std::endl;

    designBusRoute(graph);

    outFile.close();

    std::cout << "Custo total de excavacao: " << std::get<1>(result) << std::endl;

    return 0;
}