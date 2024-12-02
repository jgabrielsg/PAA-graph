// Graph.h
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
    // std::string region; // Removed or made optional
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

// Graph class
class Graph {
public:
    Graph();
    
    bool loadFromJson(const std::string& filename);
    
    const std::vector<Node>& getNodes() const;
    const std::vector<Edge>& getEdges() const;
    const std::vector<Property>& getProperties() const;
    
    void printNodes() const;
    void printEdges() const;
    void printProperties() const;

private:
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::vector<Property> properties;
};

#endif // GRAPH_H