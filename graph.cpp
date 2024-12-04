#include "Graph.h"
#include <fstream>
#include <iostream>

// Constructor
Graph::Graph() {}

// Helper function to load a field from JSON
template<typename T>
void loadField(const json& j, const std::string& key, T& field) {
    field = j[key].get<T>();
}

// Function to load graph from JSON
bool Graph::loadFromJson(const std::string& filename) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << filename << std::endl;
        return false;
    }

    json j;
    infile >> j;

    nodes.clear();
    edges.clear();
    properties.clear();
    regions.clear();

    // Load nodes
    if (j.contains("nodes")) {
        for (const auto& node_json : j["nodes"]) {
            Node node;
            loadField(node_json, "id", node.id);
            loadField(node_json, "location", node.location);
            loadField(node_json, "transport_options", node.transport_options);
            loadField(node_json, "region", node.region);
            nodes.push_back(node);
        }
    }

    // Load edges
    if (j.contains("edges")) {
        for (const auto& edge_json : j["edges"]) {
            Edge edge;
            loadField(edge_json, "from", edge.from);
            loadField(edge_json, "to", edge.to);
            loadField(edge_json, "transport_type", edge.transport_type);
            loadField(edge_json, "max_speed", edge.max_speed);
            loadField(edge_json, "distance", edge.distance);
            loadField(edge_json, "price_cost", edge.price_cost);
            loadField(edge_json, "time_cost", edge.time_cost);
            edges.push_back(edge);
        }
    }

    // Load properties
    if (j.contains("properties")) {
        for (const auto& prop_json : j["properties"]) {
            Property prop;
            loadField(prop_json, "cep", prop.cep);
            loadField(prop_json, "street", prop.street);
            loadField(prop_json, "number", prop.number);
            loadField(prop_json, "type", prop.type);
            loadField(prop_json, "from", prop.from);
            loadField(prop_json, "to", prop.to);
            properties.push_back(prop);
        }
    }

    infile.close();

    // Group nodes into regions
    groupNodesIntoRegions();

    // Debug: Imprimir algumas arestas carregadas
    std::cout << "Total de arestas carregadas: " << edges.size() << std::endl;
    for (size_t i = 0; i < std::min(edges.size(), size_t(5)); ++i) { // Imprime as primeiras 5 arestas
        std::cout << "Aresta " << i+1 << ": " << edges[i].from << " - " << edges[i].to << std::endl;
    }

    return true;
}

// Group nodes into regions
void Graph::groupNodesIntoRegions() {
    std::unordered_map<std::string, std::vector<std::string>> temp_regions;
    for (const auto& node : nodes) {
        temp_regions[node.region].push_back(node.id);
    }

    for (const auto& [region_name, node_ids] : temp_regions) {
        Region region{region_name, node_ids};
        regions.push_back(region);
    }
}

// Getters
const std::vector<Region>& Graph::getRegions() const { return regions; }
const std::vector<Node>& Graph::getNodes() const { return nodes; }
const std::vector<Edge>& Graph::getEdges() const { return edges; }
const std::vector<Property>& Graph::getProperties() const { return properties; }

// Print utility functions
void Graph::printNodes() const {
    std::cout << "Nodes:" << std::endl;
    for (const auto& node : nodes) {
        std::cout << "ID: " << node.id
                  << ", Location: (" << node.location.first << ", " << node.location.second << ")"
                  << ", Transport Options: ";
        for (const auto& option : node.transport_options) {
            std::cout << option << " ";
        }
        std::cout << std::endl;
    }
}

void Graph::printEdges() const {
    std::cout << "Edges:" << std::endl;
    for (const auto& edge : edges) {
        std::cout << "From: " << edge.from
                  << ", To: " << edge.to
                  << ", Transport: " << edge.transport_type
                  << ", Max Speed: " << edge.max_speed
                  << ", Distance: " << edge.distance
                  << ", Price Cost: " << edge.price_cost
                  << ", Time Cost: " << edge.time_cost
                  << std::endl;
    }
}

void Graph::printProperties() const {
    std::cout << "Properties:" << std::endl;
    for (const auto& prop : properties) {
        std::cout << "CEP: " << prop.cep
                  << ", Street: " << prop.street
                  << ", Number: " << prop.number
                  << ", Type: " << prop.type
                  << ", From: " << prop.from
                  << ", To: " << prop.to
                  << std::endl;
    }
}

std::unordered_map<std::string, double> Graph::getNeighbors(const std::string& nodeId) const {
    std::unordered_map<std::string, double> neighbors;

    for (const auto& edge : edges) {
        if (edge.from == nodeId) {
            neighbors[edge.to] = edge.distance;
        } else if (edge.to == nodeId) {
            neighbors[edge.from] = edge.distance; 
        }
    }

    return neighbors;
}
