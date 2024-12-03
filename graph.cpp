// Graph.cpp
#include "Graph.h"
#include <fstream>
#include <iostream>

// Constructor
Graph::Graph() {}

// Function to load graph from JSON
bool Graph::loadFromJson(const std::string& filename) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }

    json j;
    try {
        infile >> j;
    } catch (json::parse_error& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
        return false;
    }

    // Clear existing data
    nodes.clear();
    edges.clear();
    properties.clear();
    regions.clear(); // Clear existing regions

    try {
        // Load nodes
        if (j.contains("nodes") && j["nodes"].is_array()) {
            for (const auto& node_json : j["nodes"]) {
                Node node;

                // ID
                if (node_json.contains("id") && node_json["id"].is_string()) {
                    node.id = node_json["id"].get<std::string>();
                } else {
                    std::cerr << "Error: A node is missing 'id' or 'id' is not a string." << std::endl;
                    continue; // Skip this node
                }

                // Location
                if (node_json.contains("location") && node_json["location"].is_array() && node_json["location"].size() == 2) {
                    node.location.first = node_json["location"][0].get<int>();
                    node.location.second = node_json["location"][1].get<int>();
                } else {
                    std::cerr << "Error: Node " << node.id << " has invalid or missing 'location'." << std::endl;
                    continue;
                }

                // Transport Options
                if (node_json.contains("transport_options") && node_json["transport_options"].is_array()) {
                    node.transport_options = node_json["transport_options"].get<std::vector<std::string>>();
                } else {
                    std::cerr << "Error: Node " << node.id << " has invalid or missing 'transport_options'." << std::endl;
                    continue;
                }

                // Region
                if (node_json.contains("region") && node_json["region"].is_string()) {
                    node.region = node_json["region"].get<std::string>();
                } else {
                    std::cerr << "Error: Node " << node.id << " has invalid or missing 'region'." << std::endl;
                    continue;
                }

                nodes.push_back(node);
            }
        } else {
            std::cerr << "Error: JSON does not contain a valid 'nodes' array." << std::endl;
            return false;
        }

        // Load edges
        if (j.contains("edges") && j["edges"].is_array()) {
            for (const auto& edge_json : j["edges"]) {
                Edge edge;

                // From
                if (edge_json.contains("from") && edge_json["from"].is_string()) {
                    edge.from = edge_json["from"].get<std::string>();
                } else {
                    std::cerr << "Error: An edge is missing 'from' or 'from' is not a string." << std::endl;
                    continue;
                }

                // To
                if (edge_json.contains("to") && edge_json["to"].is_string()) {
                    edge.to = edge_json["to"].get<std::string>();
                } else {
                    std::cerr << "Error: An edge is missing 'to' or 'to' is not a string." << std::endl;
                    continue;
                }

                // Transport Type
                if (edge_json.contains("transport_type") && edge_json["transport_type"].is_string()) {
                    edge.transport_type = edge_json["transport_type"].get<std::string>();
                } else {
                    std::cerr << "Error: Edge from " << edge.from << " to " << edge.to << " is missing 'transport_type' or it is not a string." << std::endl;
                    continue;
                }

                // Max Speed
                if (edge_json.contains("max_speed") && edge_json["max_speed"].is_number()) {
                    edge.max_speed = edge_json["max_speed"].get<double>();
                } else {
                    std::cerr << "Error: Edge from " << edge.from << " to " << edge.to << " has invalid or missing 'max_speed'." << std::endl;
                    continue;
                }

                // Distance
                if (edge_json.contains("distance") && edge_json["distance"].is_number()) {
                    edge.distance = edge_json["distance"].get<double>();
                } else {
                    std::cerr << "Error: Edge from " << edge.from << " to " << edge.to << " has invalid or missing 'distance'." << std::endl;
                    continue;
                }

                // Price Cost
                if (edge_json.contains("price_cost") && edge_json["price_cost"].is_number()) {
                    edge.price_cost = edge_json["price_cost"].get<double>();
                } else {
                    std::cerr << "Error: Edge from " << edge.from << " to " << edge.to << " has invalid or missing 'price_cost'." << std::endl;
                    continue;
                }

                // Time Cost
                if (edge_json.contains("time_cost") && edge_json["time_cost"].is_number()) {
                    edge.time_cost = edge_json["time_cost"].get<double>();
                } else {
                    std::cerr << "Error: Edge from " << edge.from << " to " << edge.to << " has invalid or missing 'time_cost'." << std::endl;
                    continue;
                }

                edges.push_back(edge);
            }
        } else {
            std::cerr << "Error: JSON does not contain a valid 'edges' array." << std::endl;
            return false;
        }

        // Load properties if present
        if (j.contains("properties") && j["properties"].is_array()) {
            for (const auto& prop_json : j["properties"]) {
                Property prop;

                if (prop_json.contains("cep") && prop_json["cep"].is_string()) {
                    prop.cep = prop_json["cep"].get<std::string>();
                } else {
                    std::cerr << "Error: A property is missing 'cep' or 'cep' is not a string." << std::endl;
                    continue;
                }

                if (prop_json.contains("street") && prop_json["street"].is_string()) {
                    prop.street = prop_json["street"].get<std::string>();
                } else {
                    std::cerr << "Error: Property with cep " << prop.cep << " is missing 'street' or 'street' is not a string." << std::endl;
                    continue;
                }

                if (prop_json.contains("number") && prop_json["number"].is_number_integer()) {
                    prop.number = prop_json["number"].get<int>();
                } else {
                    std::cerr << "Error: Property on street " << prop.street << " is missing 'number' or 'number' is not an integer." << std::endl;
                    continue;
                }

                if (prop_json.contains("type") && prop_json["type"].is_string()) {
                    prop.type = prop_json["type"].get<std::string>();
                } else {
                    std::cerr << "Error: Property on street " << prop.street << " is missing 'type' or 'type' is not a string." << std::endl;
                    continue;
                }

                if (prop_json.contains("from") && prop_json["from"].is_string()) {
                    prop.from = prop_json["from"].get<std::string>();
                } else {
                    std::cerr << "Error: Property on street " << prop.street << " is missing 'from' or 'from' is not a string." << std::endl;
                    continue;
                }

                if (prop_json.contains("to") && prop_json["to"].is_string()) {
                    prop.to = prop_json["to"].get<std::string>();
                } else {
                    std::cerr << "Error: Property on street " << prop.street << " is missing 'to' or 'to' is not a string." << std::endl;
                    continue;
                }

                properties.push_back(prop);
            }
        }

    } catch (const json::exception& e) {
        std::cerr << "JSON error during parsing: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "General error during parsing: " << e.what() << std::endl;
        return false;
    }

    infile.close();

    // Group nodes into regions
    groupNodesIntoRegions();

    return true;
}

// Getter for regions
const std::vector<Region>& Graph::getRegions() const {
    return regions;
}

// Function to group nodes into regions based on their 'region' attribute
void Graph::groupNodesIntoRegions() {
    std::unordered_map<std::string, std::vector<std::string>> temp_regions;

    for (const auto& node : nodes) {
        temp_regions[node.region].push_back(node.id);
    }

    for (const auto& [region_name, node_ids] : temp_regions) {
        Region region;
        region.name = region_name;
        region.nodes = node_ids;
        regions.push_back(region);
    }
}

// Getters
const std::vector<Node>& Graph::getNodes() const {
    return nodes;
}

const std::vector<Edge>& Graph::getEdges() const {
    return edges;
}

const std::vector<Property>& Graph::getProperties() const {
    return properties;
}

// Utility functions
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
