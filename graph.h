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
    int region; // Added region attribute
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
    double excavation_cost;
    int num_residencial;
    int num_comercial;
    int num_touristic;
    int num_industrial;
    int bus_preference;
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
    int number;                // Region name (e.g., "Region1")
    std::vector<std::string> nodes;  // Node IDs belonging to the region
};

// Graph class
class Graph {
public:
    Graph();
    
    bool loadFromJson(const std::string& filename);
    
    const std::vector<Region>& getRegions() const ;
    const std::vector<Node>& getNodes() const ;
    const std::vector<Edge>& getEdges() const ;
    const std::vector<Property>& getProperties() const; 
    
    // New methods to add nodes, edges, and properties
    void addNode(const Node& node);
    void addEdge(const Edge& edge);
    void addProperty(const Property& property);
    
    void printNodes() const;
    void printEdges() const;
    void printProperties() const;

    // Add this declaration
    std::unordered_map<std::string, double> getNeighbors(const std::string& nodeId) const;

    // Add this new declaration for getNodeIndex
    int getNodeIndex(const std::string& nodeId) const;

    // Add this new declaration for getRegionNumber
    int getRegionNumber(const std::string& nodeId) const;

    // Add these for pathfinding
    std::vector<std::pair<std::string, std::string>> getPathFromParents(
        const std::vector<std::string>& parent, const std::string& start, const std::string& end) const;

    void findBestStationsAndPaths(std::vector<std::string>& bestStations, 
                                  std::vector<std::vector<std::string>>& parentsList) const;

    void createGraphFromBestStations(const std::vector<std::string>& bestStations,
                                     const std::vector<std::vector<std::string>>& parentsList,
                                     Graph& newGraph) const;

private:
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::vector<Property> properties;
    std::vector<Region> regions; // Stored regions

    // Helper to group nodes into regions
    void groupNodesIntoRegions();
};

#endif // GRAPH_H
