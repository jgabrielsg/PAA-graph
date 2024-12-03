#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include "external/json.hpp"

using json = nlohmann::json;

// Structure to represent a Node
struct Node {
    std::string id;
    std::pair<int, int> location;
    std::vector<std::string> transport_options;
    std::string region; // Added region attribute
};

// Structure to represent an Edge
struct Edge {
    std::string from;
    std::string to;
    std::string transport_type;
    double max_speed;
    double distance;
    double price_cost;
    double time_cost;
};

// Structure to represent a Property
struct Property {
    std::string cep;
    std::string street;
    int number;
    std::string type;
    std::string from;
    std::string to;
};

// Structure to represent a Region
struct Region {
    std::string name;               // Region name (e.g., "Region1")
    std::vector<std::string> nodes; // Node IDs belonging to the region
};

// Graph class
class Graph {
public:
    Graph();
    
    bool loadFromJson(const std::string& filename);
    
    const std::vector<Node>& getNodes() const;
    const std::vector<Edge>& getEdges() const;
    const std::vector<Property>& getProperties() const;
    
    // Added getter for regions
    const std::vector<Region>& getRegions() const;
    
    void printNodes() const;
    void printEdges() const;
    void printProperties() const;

    // Add this declaration
    std::unordered_map<std::string, double> getNeighbors(const std::string& nodeId) const;

private:
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::vector<Property> properties;
    std::vector<Region> regions; // Stored regions

    // Helper to group nodes into regions
    void groupNodesIntoRegions();
};

#endif // GRAPH_H
